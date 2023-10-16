# SimpleScheduler  
 
## Project Overview

Provide a brief overview of the project, its purpose, and the problem it aims to solve.

## SimpleScheduler Implementation

### Overview

The SimpleScheduler is a component of our project that handles CPU scheduling activities. It is responsible for managing a pool of processes and allocating CPU resources to them based on the provided parameters.

### Key Features

#### Round-Robin Scheduling
Processes are scheduled in a round-robin fashion, a well-known algorithm in scheduling. Each process is granted a fixed time quantum (TSLICE) to execute. This approach ensures that processes take turns efficiently utilizing CPU resources.

#### Shared Memory
We employ shared memory to maintain essential process queues. These queues are fundamental in enabling seamless communication between the SimpleScheduler and SimpleShell components of our project.

#### Signal Handling
Custom signal handling, including SIGUSR2, plays a significant role in the coordination and control of process execution. These signals trigger various events, allowing the scheduler to pause or terminate processes as needed.

### Implementation Details

#### Shared Memory
Shared memory is a critical component of our SimpleScheduler. It is used to maintain three essential queues:

- Waiting Queue: This queue holds processes that are ready to run but are waiting for their turn.
- Running Queue: This queue contains processes that are currently executing on available CPU resources.
- Finished Queue: Processes that have completed their execution are moved to this queue for monitoring and statistics.

Shared memory enables efficient sharing of data between the scheduler and other system components, ensuring coordinated process execution.

#### Signal Handling
Custom signal handlers are a cornerstone of our scheduler's functionality. These handlers enable the scheduler to respond to critical events:

- SIGUSR2: This signal is used to notify the scheduler to distribute CPU resources to waiting processes. It marks the end of a time quantum (TSLICE) and signals processes to stop execution.
- SIGALRM: This signal is used to control and trigger the distribution of CPU resources based on the round-robin scheduling algorithm.
- SIGUSR1: This signal is utilized to provide users with information about processes in the waiting queue.

#### Priority Queue
To ensure that processes are executed based on their priority, we implement a priority queue. This data structure allows processes to be dequeued from the ready queue based on their priority level. This ensures higher-priority processes are scheduled before lower-priority ones.

### Code Structure

The codebase of our SimpleScheduler comprises various functions and components:

- `initialize_shared_queues()`: This function initializes shared memory and the primary process queues, including the waiting, running, and finished queues.

- `custom_signal_handler()`: Custom signal handlers are defined in this function. It handles signals such as SIGUSR2 for process scheduling and suspension, SIGALRM to trigger resource distribution, and SIGUSR1 to provide information about processes in the waiting queue.

- `run_executable()`: This function initiates the execution of user-submitted processes. It creates new processes, assigns them priority, and adds them to the appropriate queues. This is the heart of the scheduler's core functionality.

The codebase of our SimpleScheduler is structured to facilitate efficient and equitable CPU scheduling while providing mechanisms for managing processes effectively. Our use of shared memory and custom signal handling ensures a smooth and coordinated execution process, and the priority queue is instrumental in scheduling processes fairly.
