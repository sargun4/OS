
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

struct CommandHistory {
    char command[MAX_INPUT_LENGTH];
    time_t timestamp;
    pid_t pid;     
    time_t start_time;  
    time_t end_time;
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


void read_input(char* input) {
    printf("SimpleShell > ");
    fflush(stdout);
    fgets(input, MAX_INPUT_LENGTH, stdin);
    input[strlen(input) - 1] = '\0'; 
}

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

    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }

    status = pclose(fp);
    if (status == -1) {
        // perror("Pclose failed");
        return -1;
    }

    return WEXITSTATUS(status);
}

// execute a command
int launch(char* command) {
    int status = create_process_and_run(command);
    return status;
}

//  add a command to the history
void add_to_history(char* command, pid_t pid) {
    if (history_count < MAX_HISTORY_SIZE) {
        strcpy(history[history_count].command, command);
        history[history_count].timestamp = time(NULL);
        history[history_count].pid = pid;  // Store the process ID
        history[history_count].start_time = time(NULL);  // Store start time
        history_count++;
    }
}

void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: [%s] (Timestamp: %s)\n", i + 1, history[i].command, asctime(localtime(&history[i].timestamp)));
        printf("Process ID: %d\n", history[i].pid);
        printf("Execution duration: %ld seconds\n", history[i].end_time - history[i].start_time);
    }
}

int main() {
    char input[MAX_INPUT_LENGTH];
    // Register the SIGCHLD signal handler
    signal(SIGCHLD, handle_sigchld);
    do {
        read_input(input);
        if (strcmp(input, "exit") == 0) {
            // Display details upon termination
            for (int i = 0; i < history_count; i++) {
                printf("Command: %s\n", history[i].command);
                printf("Timestamp: %s", asctime(localtime(&history[i].timestamp)));
                printf("Process ID: %d\n", history[i].pid);
                printf("Execution duration: %ld seconds\n", history[i].end_time - history[i].start_time);
            }
            break;
        }

        if (strcmp(input, "history") == 0) {
            show_history();
        } else {
            pid_t pid = getpid();  
            add_to_history(input, pid);
            history[history_count - 1].start_time = time(NULL);  
            int status = launch(input);
            if (status != -1) {
                history[history_count - 1].end_time = time(NULL); 
                printf("Exit status: %d\n", status);
            }
        }
    } while (1);
    return 0;
}

