# Asgn2

https://docs.google.com/document/d/1UOWEj1oHgfoPr0y4Kv44gjTCzD0oeEwdcgWfyIH7AKU/edit

## Compile the Shell Program: 
            gcc -o myshell myshell.c

## Run the Shell:
            ./myshell

### Exit the Shell:  use Ctrl+C

## Implementation Details:
### 1. Data Structures:

- The struct CommandHistory structure is used to store information about executed commands. It has the following fields:
            
                struct CommandHistory {
                    char command[MAX_INPUT_LENGTH];
                    time_t timestamp;
                    pid_t pid;
                    time_t start_time;
                    time_t end_time;
                };
                
- command: A character array to store the command itself.
timestamp: A time_t variable to store the timestamp of when the command was executed.
- pid: A pid_t variable to store the process ID (PID) of the executed command.
- start_time: A time_t variable to store the start time of command execution.
- end_time: A time_t variable to store the end time of command execution.

### 2. Signal Handling (handle_sigchld):

- This signal handler is triggered when a child process terminates.
- It uses waitpid() with the WNOHANG option to check for the termination of child processes without blocking.
- When a child process terminates, it updates child_exit_status to store the exit status of the child process.
- It also iterates through the history array to find the corresponding entry for the terminated child process and updates its end_time field.

### 3. Reading User Input (read_input):

- The read_input function prompts the user for input with "SimpleShell > ".
- It flushes the standard output to ensure the prompt is displayed immediately.
- It uses fgets() to read a line of input (command) from the user.
- It removes the trailing newline character from the input, making it easier to process using:

            input[strlen(input) - 1] = '\0';

### 4. Executing Commands (execute_command):

- This function is responsible for executing user-entered commands.
- It takes the command as input and performs the following steps:
- Creates a pipe using pipe() to establish communication between the parent and child processes.
- Forks a new process using fork(). In the child process, it executes the command.
- In the parent process, it reads the command's output from the read end of the pipe and prints it to the screen.
- It captures the child's exit status using waitpid().
- The child's PID is recorded in the history entry for the executed command.

### 5. Adding Commands to History (add_to_history):

- The add_to_history function is called after a command is executed successfully.
- It records the executed command, current timestamp, and the PID of the child process (obtained from fork()) in a history entry.
- It initializes the start_time for execution, which will be used to calculate the execution duration later.

### 6. Displaying Command History (show_history):

- The show_history function displays the command history for the current session, including additional details.
- It iterates through the history array and prints details for each command if the command has finished executing (end time is recorded).
### 7. Main Loop:

- The main function is the entry point for the shell program.
- It registers the SIGCHLD signal handler to handle child process termination.
- It enters a loop to read and execute user commands until the user decides to exit.
Within the loop:
- It reads user input using read_input.
- It allows the user to exit the shell gracefully with the "exit" - command.
- It displays the command history for the current session with the "history" command.
- It executes user-entered commands using execute_command, records them in the history, and calculates execution times.
- It prints the exit status of executed commands.

### 8. history 
- example:

            SimpleShell > history
            1: [ls] (Timestamp: Sat Sep  9 03:08:38 2023
            )
            Process ID: 90935
            Execution duration: 0 seconds
            2: [cat fib.c] (Timestamp: Sat Sep  9 03:08:44 2023
            )
            Process ID: 90985
            Execution duration: 0 seconds
            3: [cat fib.c|wc -l] (Timestamp: Sat Sep  9 03:08:51 2023
            )
            Process ID: 91010
            Execution duration: 0 seconds
            4: [cat fib.c|wc -c] (Timestamp: Sat Sep  9 03:09:00 2023
            )
            Process ID: 91077
            Execution duration: 0 seconds

Summary:

The code implements a simple shell that provides a user-friendly interface for executing commands. It captures the output of executed commands, maintains a history of commands with additional details, handles child process termination, and allows users to view their command history. It uses pipes and fork() for command execution and signal handling for child process termination.
