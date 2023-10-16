#include <stdio.h>
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
#include <sys/mman.h>

#define MAX_INPUT_LENGTH 1024
#define QUEUE_SIZE 100

typedef struct process
{
    char name[MAX_INPUT_LENGTH];
    pid_t pid;
    uint64_t execution_time;
    uint64_t waiting_time;
    int priority;
} process;

typedef struct process_queues
{
    process running_queue[QUEUE_SIZE];
    int running_queue_size;

    process waiting_queue[QUEUE_SIZE];
    int waiting_queue_size;

    process finished_queue[QUEUE_SIZE];
    int finished_queue_size;

    sem_t mutex;
} process_queues;

void heapify(process arr[], int size, int i) // heapify code from gfg https://www.geeksforgeeks.org/insertion-and-deletion-in-heaps/
{
    int largest = i;
    int l = (2 * i) + 1;
    int r = (2 * i) + 2;

    if (l < size && arr[l].priority > arr[largest].priority)
    {
        largest = l;
    }

    if (r < size && arr[r].priority > arr[largest].priority)
    {
        largest = r;
    }

    if (largest != i)
    {
        struct process temp = arr[largest];
        arr[largest] = arr[i];
        arr[i] = temp;

        heapify(arr, size, largest);
    }
}

void heapify_up(process arr[], int i) // heapify up code
{
    int parent = (i - 1) / 2;

    if (i > 0 && arr[i].priority > arr[parent].priority)
    {
        // Swap the element with its parent if it has a higher priority
        struct process temp = arr[i];
        arr[i] = arr[parent];
        arr[parent] = temp;

        // Recursively heapify the parent
        heapify_up(arr, parent);
    }
}

struct process return_max(process arr[], int *(size))
{
    process out = arr[0];
    arr[0] = arr[*(size) - 1];
    *size = *size - 1;

    heapify(arr, *(size), 0);

    return out;
}

void insert(struct process arr[], int *(size), process process)
{
    arr[*(size)] = process;
    *size = *size + 1;
    heapify_up(arr, *(size) - 1);
}
