// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// // #include <sys/wait.h>

// #define MAX_INPUT_LENGTH 1024
// #define MAX_HISTORY_SIZE 50

// char history[MAX_HISTORY_SIZE][MAX_INPUT_LENGTH];
// int history_count = 0;

// void read_input(char* input) {
//     printf("SimpleShell > "); //input prompt 
//     fflush(stdout);
//     fgets(input, MAX_INPUT_LENGTH, stdin);
//     input[strlen(input) - 1] = '\0'; // Remove the trailing newline character
// }
// void show_history() {
//     int i;
//     for (i = 0; i < history_count; i++) {
//         printf("%d: %s\n", i + 1, history[i]);
//     }
// }
// void execute_command(char* command) {
//     pid_t pid;
//     pid = fork();

//     if (pid < 0) {
//         perror("Fork failed");
//     } else if (pid == 0) {
//         // Child process
//         char* args[MAX_INPUT_LENGTH];
//         char* token = strtok(command, " ");
//         int i = 0;

//         while (token != NULL) {
//             args[i] = token;
//             token = strtok(NULL, " ");
//             i++;
//         }
//         args[i] = NULL;

//         execvp(args[0], args);
//         perror("Exec failed");
//         exit(EXIT_FAILURE);
//     } else {
//         // Parent process
//         wait(NULL);
//     }
// }

// int main() {
//     char input[MAX_INPUT_LENGTH];
//     while (1) {
//         read_input(input);

//         if (strcmp(input, "exit") == 0) {
//             break;
//         }

//         if (strcmp(input, "history") == 0) {
//             for (int i = 0; i < history_count; i++) {
//                 printf("%d: %s\n", i + 1, history[i]);
//             }
//         } else {
//             // Execute the command
//             strcpy(history[history_count % MAX_HISTORY_SIZE], input);
//             history_count++;
//             execute_command(input);
//         }
//     }

//     return 0;
// }


// // It reads user input and parses it into commands and arguments.
// // It executes the commands using execvp() in child processes.
// // It supports the history command to display command history with timestamps.
// // It displays additional details (process ID, timestamp, execution duration) upon termination.
// // It maintains a history of executed commands (up to a maximum limit) along with timestamps.
// // It uses the time()capture timestamps for command execution.



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

// // Structure to store command history
// struct CommandHistory {
//     char command[MAX_INPUT_LENGTH];
//     time_t timestamp;
// };

// struct CommandHistory history[MAX_HISTORY_SIZE];
// int history_count = 0;

// // Signal handler for SIGCHLD
// void handle_sigchld(int signal) {
//     int status;
//     waitpid(-1, &status, WNOHANG);
//     if (WIFEXITED(status)) {
//         child_exit_status = WEXITSTATUS(status);
//     }
// }

// //read user input
// void read_input(char* input) {
//     printf("SimpleShell > ");
//     fflush(stdout);
//     fgets(input, MAX_INPUT_LENGTH, stdin);
//     input[strlen(input) - 1] = '\0'; // Remove the trailing newline character
// }

// //create a process and run the command
// int create_process_and_run(char* command) {
//     pid_t pid;
//     int status;

//     pid = fork();

//     if (pid < 0) {
//         perror("Fork failed");
//         return -1;
//     } else if (pid == 0) {
//         // Child process
//         char* args[MAX_INPUT_LENGTH];
//         char* token = strtok(command, " ");
//         int i = 0;
//         while (token != NULL) {
//             args[i] = token;
//             token = strtok(NULL, " ");
//             i++;
//         }
//         args[i] = NULL;

//         execvp(args[0], args);

//         perror("Exec failed");
//         exit(EXIT_FAILURE);
//     } else {
//         // Parent process
//         waitpid(pid, &status, 0);
//         return status;
//     }
// }
// //execute a command
// int launch(char* command) {
//     int status = create_process_and_run(command);
//     return status;
// }

// //add a command to the history
// void add_to_history(char* command) {
//     if (history_count < MAX_HISTORY_SIZE) {
//         strcpy(history[history_count].command, command);
//         history[history_count].timestamp = time(NULL);
//         history_count++;
//     }
// }

// //display command history
// void show_history() {
//     for (int i = 0; i < history_count; i++) {
//         printf("%d: [%s] (Timestamp: %s)\n", i + 1, history[i].command, asctime(localtime(&history[i].timestamp)));
//     }
// }

// int main() {
//     char input[MAX_INPUT_LENGTH];
//     // Register the SIGCHLD signal handler
//     signal(SIGCHLD, handle_sigchld);
//     do {
//         read_input(input);
//         if (strcmp(input, "exit") == 0) {
//             //display on termination
//             for (int i = 0; i < history_count; i++) {
//                 printf("Command: %s\n", history[i].command);
//                 printf("Timestamp: %s", asctime(localtime(&history[i].timestamp)));
//                 printf("Execution duration: %ld seconds\n", time(NULL) - history[i].timestamp);
//             }
//             break;
//         }
//         if (strcmp(input, "history") == 0) {
//             show_history();
//         } else {
//             // Execute the command, add it to history
//             add_to_history(input);
//             int status = launch(input);
//             if (status != -1) {
//                 printf("Exit status: %d\n", status);
//             }
//         }
//     } while (1);
//     return 0;
// }


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

// // Structure to store command history
// struct CommandHistory {
//     char command[MAX_INPUT_LENGTH];
//     time_t timestamp;
// };

// struct CommandHistory history[MAX_HISTORY_SIZE];
// int history_count = 0;

// // Signal handler for SIGCHLD
// void handle_sigchld(int signal) {
//     int status;
//     waitpid(-1, &status, WNOHANG);
//     if (WIFEXITED(status)) {
//         child_exit_status = WEXITSTATUS(status);
//     }
// }

// // Function to read user input
// void read_input(char* input) {
//     printf("SimpleShell > ");
//     fflush(stdout);
//     fgets(input, MAX_INPUT_LENGTH, stdin);
//     input[strlen(input) - 1] = '\0'; // Remove the trailing newline character
// }


// // Function to create a process and run the command
// int create_process_and_run(char* command) {
//     pid_t pid;
//     int status;

//     pid = fork();

//     if (pid < 0) {
//         perror("Fork failed");
//         return -1;
//     } else if (pid == 0) {
//         // Child process
//         char* args[MAX_INPUT_LENGTH];
//         char* token = strtok(command, " ");
//         int i = 0;
//         while (token != NULL) {
//             args[i] = token;
//             token = strtok(NULL, " ");
//             i++;
//         }
//         args[i] = NULL;

//         execvp(args[0], args);

//         perror("Exec failed");
//         exit(EXIT_FAILURE);
//     } else {
//         // Parent process
//         waitpid(pid, &status, 0);
//         return status;
//     }
// }

// // Function to add a command to the history
// void add_to_history(char* command) {
//     if (history_count < MAX_HISTORY_SIZE) {
//         strcpy(history[history_count].command, command);
//         history[history_count].timestamp = time(NULL);
//         history_count++;
//     }
// }

// // Function to execute a command
// int launch(char* command) {
//     int status = create_process_and_run(command);
//     return status;
// }

// // Function to display command history
// void show_history() {
//     for (int i = 0; i < history_count; i++) {
//         printf("%d: [%s] (Timestamp: %s)\n", i + 1, history[i].command, asctime(localtime(&history[i].timestamp)));
//     }
// }

// int main() {
//     char input[MAX_INPUT_LENGTH];
//     // Register the SIGCHLD signal handler
//     signal(SIGCHLD, handle_sigchld);
//     do {
//         read_input(input);
//         if (strcmp(input, "exit") == 0) {
//             // Display on termination
//             for (int i = 0; i < history_count; i++) {
//                 printf("Command: %s\n", history[i].command);
//                 printf("Timestamp: %s", asctime(localtime(&history[i].timestamp)));
//                 printf("Execution duration: %ld seconds\n", time(NULL) - history[i].timestamp);
//             }
//             break;
//         }
//         if (strcmp(input, "history") == 0) {
//             show_history();
//         } else {
//             // Execute the command, add it to history
//             add_to_history(input);
//             int status = launch(input);
//             if (status != -1) {
//                 printf("Exit status: %d\n", status);
//             }
//         }
//     } while (1);
//     return 0;
// }




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

// // Structure to store command history
// struct CommandHistory {
//     char command[MAX_INPUT_LENGTH];
//     time_t timestamp;
// };

// struct CommandHistory history[MAX_HISTORY_SIZE];
// int history_count = 0;

// // Signal handler for SIGCHLD
// void handle_sigchld(int signal) {
//     int status;
//     waitpid(-1, &status, WNOHANG);
//     if (WIFEXITED(status)) {
//         child_exit_status = WEXITSTATUS(status);
//     }
// }

// // Function to read user input
// void read_input(char* input) {
//     printf("SimpleShell > ");
//     fflush(stdout);
//     fgets(input, MAX_INPUT_LENGTH, stdin);
//     input[strlen(input) - 1] = '\0'; // Remove the trailing newline character
// }

// // Function to execute a command and display additional details
// void execute_command(char* command) {
//     pid_t pid;
//     int status;

//     pid = fork();

//     if (pid < 0) {
//         perror("Fork failed");
//     } else if (pid == 0) {
//         // Child process
//         char* args[MAX_INPUT_LENGTH];
//         char* token = strtok(command, " ");
//         int i = 0;
//         while (token != NULL) {
//             args[i] = token;
//             token = strtok(NULL, " ");
//             i++;
//         }
//         args[i] = NULL;

//         // Record the start time
//         time_t start_time = time(NULL);

//         execvp(args[0], args);

//         perror("Exec failed");
//         exit(EXIT_FAILURE);
//     } else {
//         // Parent process
//         waitpid(pid, &status, 0);

//         // Record the end time
//         time_t end_time = time(NULL);

//         if (WIFEXITED(status)) {
//             int exit_status = WEXITSTATUS(status);

//             printf("Command executed with process ID: %d\n", pid);
//             printf("Timestamp: %s", asctime(localtime(&end_time)));
//             printf("Execution duration: %ld seconds\n", end_time - history[history_count - 1].timestamp);
//             printf("Exit status: %d\n", exit_status);
//         } else {
//             printf("Command execution failed.\n");
//         }
//     }
// }

// // Function to execute a command
// int launch(char* command) {
//     execute_command(command);
//     return child_exit_status;
// }

// // Function to add a command to the history
// void add_to_history(char* command) {
//     if (history_count < MAX_HISTORY_SIZE) {
//         strcpy(history[history_count].command, command);
//         history[history_count].timestamp = time(NULL);
//         history_count++;
//     }
// }

// // Function to display command history
// void show_history() {
//     for (int i = 0; i < history_count; i++) {
//         printf("%d: [%s] (Timestamp: %s)\n", i + 1, history[i].command, asctime(localtime(&history[i].timestamp)));
//     }
// }

// int main() {
//     char input[MAX_INPUT_LENGTH];
//     // Register the SIGCHLD signal handler
//     signal(SIGCHLD, handle_sigchld);
//     do {
//         read_input(input);
//         if (strcmp(input, "exit") == 0) {
//             // Display on termination
//             for (int i = 0; i < history_count; i++) {
//                 printf("Command: %s\n", history[i].command);
//                 printf("Timestamp: %s", asctime(localtime(&history[i].timestamp)));
//                 printf("Execution duration: %ld seconds\n", time(NULL) - history[i].timestamp);
//             }
//             break;
//         }
//         if (strcmp(input, "history") == 0) {
//             show_history();
//         } else {
//             // Execute the command, add it to history
//             add_to_history(input);
//             int status = launch(input);
//             if (status != -1) {
//                 printf("Exit status: %d\n", status);
//             }
//         }
//     } while (1);
//     return 0;
// }
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_HISTORY_SIZE 50

volatile sig_atomic_t child_exit_status = 0;

// Structure to store command history
struct CommandHistory {
    char command[MAX_INPUT_LENGTH];
    time_t timestamp;
};

struct CommandHistory history[MAX_HISTORY_SIZE];
int history_count = 0;

// Signal handler for SIGCHLD
void handle_sigchld(int signal) {
    int status;
    waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        child_exit_status = WEXITSTATUS(status);
    }
}

// Function to read user input
void read_input(char* input) {
    printf("SimpleShell > ");
    fflush(stdout);
    fgets(input, MAX_INPUT_LENGTH, stdin);
    input[strlen(input) - 1] = '\0'; // Remove the trailing newline character
}


// Function to create a process and run the command
int create_process_and_run(char* command) {
    FILE* fp;
    char line[MAX_INPUT_LENGTH];
    int status;

    // Use popen to run the command and capture its output
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Popen failed");
        return -1;
    }

    // Read and display the command output
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }

    status = pclose(fp);
    if (status == -1) {
        perror("Pclose failed");
        return -1;
    }

    return WEXITSTATUS(status);
}



// Function to execute a command
int launch(char* command) {
    int status = create_process_and_run(command);
    return status;
}

// Function to add a command to the history
void add_to_history(char* command) {
    if (history_count < MAX_HISTORY_SIZE) {
        strcpy(history[history_count].command, command);
        history[history_count].timestamp = time(NULL);
        history_count++;
    }
}

// Function to display command history
void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: [%s] (Timestamp: %s)\n", i + 1, history[i].command, asctime(localtime(&history[i].timestamp)));
    }
}

int main() {
    char input[MAX_INPUT_LENGTH];
    // Register the SIGCHLD signal handler
    signal(SIGCHLD, handle_sigchld);
    do {
        read_input(input);
        if (strcmp(input, "exit") == 0) {
            // Display on termination
            for (int i = 0; i < history_count; i++) {
                printf("Command: %s\n", history[i].command);
                printf("Timestamp: %s", asctime(localtime(&history[i].timestamp)));
                printf("Execution duration: %ld seconds\n", time(NULL) - history[i].timestamp);
            }
            break;
        }
        if (strcmp(input, "history") == 0) {
            show_history();
        } else {
            // Execute the command, add it to history
            add_to_history(input);
            int status = launch(input);
            if (status != -1) {
                printf("Exit status: %d\n", status);
            }
        }
    } while (1);
    return 0;
}
