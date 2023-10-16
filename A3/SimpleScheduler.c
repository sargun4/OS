#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "SimpleScheduler.h"

process_queues *shared_queues;

int fd;
int NCPU;
int TSLICE;

void initialize_shared_queues()
{
    fd = shm_open("SharedMemory", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sizeof(process_queues)) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shared_queues = mmap(NULL, sizeof(process_queues), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shared_queues == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&(shared_queues->mutex), 1, 1) == -1)
    {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
}

void cleanup()
{
    if (munmap(shared_queues, sizeof(process_queues)) == -1)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1)
    {
        perror("close");
        exit(EXIT_FAILURE);
    }

    if (sem_destroy(&(shared_queues->mutex)) == -1)
    {
        perror("sem_destroy");
        exit(EXIT_FAILURE);
    }

    if (shm_unlink("SharedMemory") == -1)
    {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }
}

void custom_signal_handler(int signo)
{

    printf("Custom signal %d received.\n", signo);
    // Handle the custom signal as needed

    if (signo == SIGUSR2)
    {
        if (shared_queues->running_queue_size > 0)
        {
            for (int i = 0; i < NCPU; i++)
            {
                sem_wait(&((shared_queues->mutex)));
                process process = return_max(shared_queues->running_queue, &(shared_queues->running_queue_size));
                sem_post(&(shared_queues->mutex));

                kill(process.pid, SIGSTOP);

                int status;
                pid_t terminated_pid;

                terminated_pid = waitpid(process.pid, &status, WNOHANG);

                if (terminated_pid == -1)
                {
                    perror("waitpid");
                    exit(EXIT_FAILURE);
                }

                if (terminated_pid == 0)
                {
                    sem_wait(&((shared_queues->mutex)));
                    insert(shared_queues->waiting_queue, &(shared_queues->waiting_queue_size), process);
                    process.waiting_time = process.waiting_time + TSLICE;
                    sem_post(&(shared_queues->mutex));

                    continue;
                }

                if (WIFEXITED(status))
                {
                    sem_wait(&((shared_queues->mutex)));
                    insert(shared_queues->finished_queue, &(shared_queues->finished_queue_size), process);
                    sem_post(&(shared_queues->mutex));
                }
            }
        }

        if (shared_queues->waiting_queue_size > 0)
        {
            if (shared_queues->waiting_queue_size >= NCPU)
            {
                for (int i = 0; i < NCPU; i++)
                {
                    sem_wait(&((shared_queues->mutex)));
                    process process = return_max(shared_queues->waiting_queue, &(shared_queues->waiting_queue_size));

                    kill(process.pid, SIGCONT);

                    insert(shared_queues->running_queue, &(shared_queues->running_queue_size), process);
                    process.execution_time = process.execution_time + TSLICE;
                    sem_post(&(shared_queues->mutex));
                }
            }

            else if (shared_queues->waiting_queue_size < NCPU)
            {
                for (int i = 0; i < shared_queues->waiting_queue_size; i++)
                {
                    sem_wait(&((shared_queues->mutex)));
                    process process = return_max(shared_queues->waiting_queue, &(shared_queues->waiting_queue_size));

                    kill(process.pid, SIGCONT);

                    insert(shared_queues->running_queue, &(shared_queues->running_queue_size), process);
                    process.execution_time = process.execution_time + TSLICE;
                    sem_post(&(shared_queues->mutex));
                }
            }

            ualarm(TSLICE * 1000, 0);
        }
    }

    else if (signo == SIGALRM)
    {
        kill(getpid(), SIGUSR2);
    }

    else if (signo == SIGUSR1)
    {
        for (int i = 0; i < shared_queues->waiting_queue_size; i++)
        {
            int status = fork();
            pid_t child_pid;

            if (status == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (status == 0)
            {
                child_pid = getpid();

                sem_wait(&((shared_queues->mutex)));
                shared_queues->waiting_queue[i].pid = child_pid;
                sem_post(&(shared_queues->mutex));

                char path[] = "./";
                strcat(path, shared_queues->waiting_queue[i].name);

                kill(child_pid, SIGSTOP);

                execl(path, shared_queues->waiting_queue[i].name, NULL);

                perror("execl");
                exit(EXIT_FAILURE);
            }
        }

        kill(getpid(), SIGUSR2);
    }

    else if (signo == SIGINT)
    {
        printf("\n");
        printf("Scheduling History: \n\n");

        for (int i = 0; i < shared_queues->finished_queue_size; i++)
        {
            process process = shared_queues->finished_queue[i];
            printf("Name: %s\nPID: %d\nPriority: %d\nExecution Time: %d\nWaiting Time: %d\n\n", process.name, process.pid, process.priority, process.execution_time, process.waiting_time);
        }

        cleanup();
        exit(0);
    }
}

int main(int argc, char const *argv[])
{
    // struct sigaction sig;
    // memset(&sig, 0, sizeof(sig));
    // sig.sa_handler = custom_signal_handler;
    // sigaction(SIGUSR2, &sig, NULL);
    // sigaction(SIGALRM, &sig, NULL);
    // sigaction(SIGUSR1, &sig, NULL);

    signal(SIGUSR1, custom_signal_handler);
    signal(SIGUSR2, custom_signal_handler);
    signal(SIGALRM, custom_signal_handler);

    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]);

    initialize_shared_queues();

    sem_wait(&((shared_queues->mutex)));
    shared_queues->finished_queue_size = 0;
    shared_queues->waiting_queue_size = 0;
    shared_queues->running_queue_size = 0;
    sem_post(&(shared_queues->mutex));

    while (1)
    {
        // wth
    }

    return 0;
}
