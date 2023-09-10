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

struct CommandHistory {
    char command[MAX_INPUT_LENGTH];
    time_t timestamp;
    pid_t pid;
    struct timeval start_time;
    struct timeval end_time;
};

struct CommandHistory history[MAX_HISTORY_SIZE];
int history_count = 0;


long long time_difference_us(struct timeval start, struct timeval end) {
    long long start_time = start.tv_sec * 1000000LL + start.tv_usec;
    long long end_time = end.tv_sec * 1000000LL + end.tv_usec;
    return end_time - start_time; //time diff in microsecs
}

void read_input(char* input) {
    printf("SimpleShell > ");
    fflush(stdout);

    if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
        perror("fgets failed");
        exit(EXIT_FAILURE);
    }

    input[strlen(input) - 1] = '\0';
}

int execute_command(char* command) {
    int pipe_fd[2];
    int status;
    pid_t child_pid;

    if (pipe(pipe_fd) == -1) {
        perror("Pipe failed");
        return -1;
    }

    if ((child_pid = fork()) == -1) {
        perror("Fork failed");
        return -1;
    }

    if (child_pid == 0) {
        close(pipe_fd[0]);

        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        execlp("sh", "sh", "-c", command, NULL);

        // Exec failed
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else {
        close(pipe_fd[1]);

        char line[MAX_INPUT_LENGTH];
        FILE* pipe_read = fdopen(pipe_fd[0], "r");

        struct CommandHistory new_entry;
        strcpy(new_entry.command, command);
        new_entry.timestamp = time(NULL);
        new_entry.pid = child_pid;
        gettimeofday(&new_entry.start_time, NULL);

        if (history_count < MAX_HISTORY_SIZE) {
            history[history_count++] = new_entry;
        }

        while (fgets(line, sizeof(line), pipe_read) != NULL) {
            printf("%s", line);
        }

        if (fclose(pipe_read) == EOF) {
            perror("fclose failed");
        }

        waitpid(child_pid, &status, 0);
        gettimeofday(&history[history_count - 1].end_time, NULL);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}

void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: [%s] (Timestamp: %s)\n", i + 1, history[i].command, asctime(localtime(&history[i].timestamp)));
        printf("Process ID: %d\n", history[i].pid);
        printf("Execution duration: %lld microseconds\n", time_difference_us(history[i].start_time, history[i].end_time));
    }
}

int main() {
    char input[MAX_INPUT_LENGTH];
    do {
        read_input(input);
        if (strcmp(input, "exit") == 0) {
            show_history();
            break;
        }

        if (strcmp(input, "history") == 0) {
            show_history();
        } else {
            int status = execute_command(input);
            if (status != -1) {
                printf("Exit status: %d\n", status);
            } else {
                perror("Command execution failed");
            }
        }
    } while (1);
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <signal.h>
// #include <time.h>

// #define MAX_INPUT_LENGTH 1024
// #define MAX_HISTORY_SIZE 50

// volatile sig_atomic_t child_exit_status = 0;

// struct CommandHistory {
//     char command[MAX_INPUT_LENGTH];
//     time_t timestamp;
//     pid_t pid;
//     time_t start_time;
//     time_t end_time;
// };

// struct CommandHistory history[MAX_HISTORY_SIZE];
// int history_count = 0;

// void handle_sigchld(int signal) {
//     int status;
//     pid_t pid;

//     while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
//         for (int i = 0; i < history_count; i++) {
//             if (history[i].pid == pid) {
//                 history[i].end_time = time(NULL); 
//                 break;
//             }
//         }
//     }

//     if (WIFEXITED(status)) {
//         child_exit_status = WEXITSTATUS(status);
//     }
// }
// void add_to_history(char* command, pid_t pid) {
//     if (history_count < MAX_HISTORY_SIZE) {
//         strcpy(history[history_count].command, command);
//         history[history_count].timestamp = time(NULL);
//         history[history_count].pid = pid;
//         history[history_count].start_time = time(NULL);
//         history_count++;
//     }
// }

// void read_input(char* input) {
//     printf("SimpleShell > ");
//     fflush(stdout);
//     fgets(input, MAX_INPUT_LENGTH, stdin);
//     input[strlen(input) - 1] = '\0';
// }
// int execute_command(char* command) {
//     int pipe_fd[2];
//     int status;
//     pid_t child_pid;

//     if (pipe(pipe_fd) == -1) {
//         perror("Pipe failed");
//         return -1;
//     }

//     if ((child_pid = fork()) == -1) {
//         perror("Fork failed");
//         return -1;
//     }

//     if (child_pid == 0) {
//         // Child process
//         close(pipe_fd[0]);

//         dup2(pipe_fd[1], STDOUT_FILENO);
//         close(pipe_fd[1]);

//         execlp("sh", "sh", "-c", command, NULL);
//         perror("Exec failed");
//         exit(EXIT_FAILURE);
//     } else {
//         // Parent process
//         close(pipe_fd[1]); 

//         char line[MAX_INPUT_LENGTH];
//         FILE* pipe_read = fdopen(pipe_fd[0], "r");

//         add_to_history(command, child_pid);

//         while (fgets(line, sizeof(line), pipe_read) != NULL) {
//             printf("%s", line);
//         }

//         fclose(pipe_read);

//         waitpid(child_pid, &status, 0);
//         if (WIFEXITED(status)) {
//             return WEXITSTATUS(status);
//         } else {
//             return -1;
//         }
//     }
// }


// void show_history() {
//     for (int i = 0; i < history_count; i++) {
//         if (history[i].end_time > 0) {
//             printf("%d: [%s] (Timestamp: %s)\n", i + 1, history[i].command, asctime(localtime(&history[i].timestamp)));
//             printf("Process ID: %d\n", history[i].pid);
//             printf("Execution duration: %ld seconds\n", history[i].end_time - history[i].start_time);
//         }
//     }
// }
// int main() {
//     char input[MAX_INPUT_LENGTH];
//     signal(SIGCHLD, handle_sigchld);
//     do {
//         read_input(input);
//         if (strcmp(input, "exit") == 0) {
//             for (int i = 0; i < history_count; i++) {
//                 printf("Command: %s\n", history[i].command);
//                 printf("Timestamp: %s", asctime(localtime(&history[i].timestamp)));
//                 printf("Process ID: %d\n", history[i].pid);
//                 printf("Execution duration: %ld seconds\n", history[i].end_time - history[i].start_time);
//             }
//             break;
//         }

//         if (strcmp(input, "history") == 0) {
//             show_history();
            
//         } else {
//             int status = execute_command(input);
//             if (status != -1) {
//                 history[history_count - 1].end_time = time(NULL);
//                 printf("Exit status: %d\n", status);
//             }
//         }
//     } while (1);
//     return 0;
// }


