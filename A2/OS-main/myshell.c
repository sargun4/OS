#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_HISTORY_SIZE 50

volatile sig_atomic_t child_exit_status = 0;

struct CommandHistory
{
    char command[MAX_INPUT_LENGTH];
    time_t timestamp;
    pid_t pid;
    struct timeval start_time;
    struct timeval end_time;
};

struct BackgroundProcesses
{
    char command[MAX_INPUT_LENGTH];
    pid_t pid;
    int is_finished;
};

struct CommandHistory history[MAX_HISTORY_SIZE];
struct BackgroundProcesses background_history[MAX_HISTORY_SIZE];

int history_count = 0;
int background_history_count = 0;

long long time_diff(struct timeval start, struct timeval end)
{
    long long start_time = start.tv_sec * 1000000LL + start.tv_usec;
    long long end_time = end.tv_sec * 1000000LL + end.tv_usec;
    return end_time - start_time; // time diff in microsecs
}

void read_input(char *input)
{
    printf("User@DESKTOP:~$ ");
    fflush(stdout);

    if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL)
    {
        perror("fgets failed");
        exit(EXIT_FAILURE);
    }

    input[strlen(input) - 1] = '\0';
}

int read_from_file(char *input, FILE *fptr)
{
    fflush(stdout);

    if (fgets(input, MAX_INPUT_LENGTH, fptr) == NULL)
    {
        if (feof(fptr))
        {
            return 0;
        }

        else
        {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }
    }

    if (input[strlen(input) - 1] == '\n')
    {
        input[strlen(input) - 1] = '\0';
    }

    return 1;
}

int execute_command(char *command, int background)
{
    if (background == 0)
    {
        int pipe_fd[2];
        int status;
        pid_t child_pid;

        if (pipe(pipe_fd) == -1)
        {
            perror("Pipe failed");
            return -1;
        }

        if ((child_pid = fork()) == -1)
        {
            perror("Fork failed");
            return -1;
        }

        if (child_pid == 0)
        {
            close(pipe_fd[0]);

            dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[1]);

            execlp("sh", "sh", "-c", command, NULL);

            // Exec failed
            perror("Exec failed");
            exit(EXIT_FAILURE);
        }

        else
        {
            close(pipe_fd[1]);

            char line[MAX_INPUT_LENGTH];
            FILE *pipe_read = fdopen(pipe_fd[0], "r");

            struct CommandHistory new_entry;
            strcpy(new_entry.command, command);
            new_entry.timestamp = time(NULL);
            new_entry.pid = child_pid;
            gettimeofday(&new_entry.start_time, NULL);

            if (history_count < MAX_HISTORY_SIZE)
            {
                history[history_count++] = new_entry;
            }

            while (fgets(line, sizeof(line), pipe_read) != NULL)
            {
                printf("%s", line);
            }

            if (fclose(pipe_read) == EOF)
            {
                perror("fclose failed");
            }

            waitpid(child_pid, &status, 0);
            gettimeofday(&history[history_count - 1].end_time, NULL);

            if (WIFEXITED(status))
            {
                return WEXITSTATUS(status);
            }

            else
            {
                return -1;
            }
        }
    }

    else if (background == 1)
    {
        int status;
        pid_t child_pid;

        if ((child_pid = fork()) == -1)
        {
            perror("Fork failed");
            return -1;
        }

        if (child_pid == 0)
        {
            execlp("sh", "sh", "-c", command, NULL);

            // Exec failed
            perror("Exec failed");
            exit(EXIT_FAILURE);
        }

        else
        {
            struct BackgroundProcesses new_entry;
            strcpy(new_entry.command, command);
            new_entry.pid = child_pid;

            if (background_history_count < MAX_HISTORY_SIZE)
            {
                background_history[background_history_count++] = new_entry;
            }

            printf("Process running in background (PID: %d)\n", child_pid);
            return 0;
        }
    }
}

void show_history()
{
    for (int i = 0; i < history_count; i++)
    {
        printf("%d: [%s] (Timestamp: %s)\n", i + 1, history[i].command, asctime(localtime(&history[i].timestamp)));
        printf("Process ID: %d\n", history[i].pid);
        printf("Execution duration: %lld microseconds\n", time_diff(history[i].start_time, history[i].end_time));
    }
}

void check_background_processes()
{
    for (int i = 0; i < background_history_count; i++)
    {
        pid_t pid = background_history[i].pid;
        int status;

        if (waitpid(pid, &status, WNOHANG) > 0)
        {
            background_history[i].is_finished = 1;
        }
    }

    for (int i = 0; i < background_history_count; i++)
    {
        printf("%d: [%s]\n", i + 1, background_history[i].command);
        printf("Process ID: %d\n", background_history[i].pid);

        if (background_history->is_finished)
            printf("Process Completed\n");

        else
            printf("Process Running\n");
    }
}

int execute_file()
{
    FILE *fptr;

    char fname[MAX_INPUT_LENGTH];
    printf("Enter the file name: ");
    fgets(fname, MAX_INPUT_LENGTH, stdin);
    fname[strlen(fname) - 1] = '\0';

    fptr = fopen(fname, "r");

    if (fptr == NULL)
    {
        perror("fopen failed");
        exit(0);
    }

    char input[MAX_INPUT_LENGTH];
    int background = 0;

    while (read_from_file(input, fptr))
    {
        if ((input[strlen(input) - 1] == '&') && (input[strlen(input) - 2] == ' '))
        {
            input[strlen(input) - 2] = '\0';
            background = 1;
        }

        else
            background = 0;

        if (strcmp(input, "exit") == 0)
        {
            show_history();
            break;
        }

        else if (strcmp(input, "check bgp") == 0)
        {
            check_background_processes();
        }

        else if (strcmp(input, "history") == 0)
        {
            show_history();
        }

        else
        {
            int status = execute_command(input, background);
            if (status != -1)
            {
                printf("Exit status: %d\n", status);
            }
            else
            {
                perror("Command execution failed");
            }
        }
    }

    fclose(fptr);
    printf("File executed Successfully!\n");
    return 0;
}

int main()
{
    char input[MAX_INPUT_LENGTH];
    int background = 0;

    do
    {
        read_input(input);

        if ((input[strlen(input) - 1] == '&') && (input[strlen(input) - 2] == ' '))
        {
            input[strlen(input) - 2] = '\0';
            background = 1;
        }

        else
            background = 0;

        if (strcmp(input, "exit") == 0)
        {
            show_history();
            break;
        }

        else if (strcmp(input, "execute file") == 0)
        {
            execute_file();
        }

        else if (strcmp(input, "check bgp") == 0)
        {
            check_background_processes();
        }

        else if (strcmp(input, "history") == 0)
        {
            show_history();
        }

        else
        {
            int status = execute_command(input, background);
            if (status != -1)
            {
                printf("Exit status: %d\n", status);
            }
            else
            {
                perror("Command execution failed");
            }
        }
    } while (1);
    return 0;
}