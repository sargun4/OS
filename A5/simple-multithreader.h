#include <chrono>
#include <iostream>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <pthread.h>

using namespace std;

struct ThreadData
{
  int id;
  int low;
  int high;
  function<void(int)> lambda;
};

static void parallel_for(int low, int high, function<void(int)> &&lambda, int numThreads)
{
  auto start_time = chrono::high_resolution_clock::now();

  pthread_t threads_arr[(numThreads - 1)];
  ThreadData thread_data_arr[(numThreads - 1)];

  auto helper = [](void *arg) -> void *
  {
    ThreadData *data = (ThreadData *)(arg);
    for (int i = data->low; i < data->high; ++i)
    {
      data->lambda(i);
    }
    pthread_exit(nullptr);
    return nullptr;
  };

  // possiibility of remainder
  int range = high - low;
  int chunksize = range / (numThreads - 1);
  int remainder = range % (numThreads - 1);

  for (int i = 0; i < (numThreads - 1); i++)
  {
    int extra = (i == (numThreads - 1) - 1) ? remainder : 0; // for extra memory chunks that cudnt be accessed by threads since loop runs tillnumthreads n it

    thread_data_arr[i].id = i;
    thread_data_arr[i].low = low + i * chunksize;
    thread_data_arr[i].high = low + (i + 1) * chunksize + extra;
    thread_data_arr[i].lambda = lambda;

    int createThreadResult = pthread_create(&threads_arr[i], nullptr, helper, (void *)&thread_data_arr[i]);

    if (createThreadResult != 0)
    {
      cerr << "Error creating thread: " << createThreadResult << endl;
      // Handle the error, possibly by terminating other threads and cleaning up.
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < (numThreads - 1); i++)
  {
    int joinThreadResult = pthread_join(threads_arr[i], nullptr);

    if (joinThreadResult != 0)
    {
      cerr << "Error joining thread: " << joinThreadResult << endl;
      // Handle the error, possibly by terminating other threads and cleaning up.
      exit(EXIT_FAILURE);
    }
  }

  auto end_time = chrono::high_resolution_clock::now();
  chrono::duration<double> duration = end_time - start_time;

  cout << "Execution time: " << duration.count() << " seconds\n";
}

struct ThreadData2D
{
  int id;
  int low;
  int high;
  int size;
  function<void(int, int)> lambda;
  pthread_mutex_t *mutex;
};

static void parallel_for(int low1, int high1, int low2, int high2, function<void(int, int)> &&lambda, int numThreads)
{
  auto start_time = chrono::high_resolution_clock::now();

  pthread_t threads_arr[(numThreads - 1)];
  ThreadData2D thread_data_arr[(numThreads - 1)];

  auto helper2d = [](void *arg) -> void *
  {
    ThreadData2D *data = (ThreadData2D *)(arg);
    for (int i = data->low; i < data->high; i++)
    {
      for (int j = 0; j < data->size; j++)
      {
        data->lambda(i, j);
      }
    }
    pthread_exit(nullptr);
    return nullptr;
  };

  int range = high1 - low1;
  int chunksize1 = range / (numThreads - 1);
  int remainder = range % (numThreads - 1);

  for (int i = 0; i < (numThreads - 1); i++)
  {
    // for the last thread to ensure that any remining elements are assigned to the last thread
    // last thread takes care of the remaining elements that didn't get evenly divided among threads
    int extra1 = (i == (numThreads - 1) - 1) ? remainder : 0;

    thread_data_arr[i].id = i;
    thread_data_arr[i].low = low1 + i * chunksize1;
    thread_data_arr[i].high = low1 + (i + 1) * chunksize1 + extra1;
    thread_data_arr[i].size = high2;
    thread_data_arr[i].lambda = lambda;

    int createThreadResult = pthread_create(&threads_arr[i], nullptr, helper2d, &thread_data_arr[i]);

    if (createThreadResult != 0)
    {
      cerr << "Error creating thread: " << createThreadResult << endl;
      // Handle the error, possibly by terminating other threads and cleaning up.
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < (numThreads - 1); i++)
  {
    int joinThreadResult = pthread_join(threads_arr[i], nullptr);

    if (joinThreadResult != 0)
    {
      cerr << "Error joining thread: " << joinThreadResult << endl;
      // Handle the error, possibly by terminating other threads and cleaning up.
      exit(EXIT_FAILURE);
    }
  }

  auto end_time = chrono::high_resolution_clock::now();
  chrono::duration<double> duration = end_time - start_time;

  cout << "Execution time: " << duration.count() << " seconds\n";
}

int user_main(int argc, char **argv);

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    cerr << "Usage: " << argv[0] << " <num_threads> <size> \n";
    return 1;
  }

  int numThread = atoi(argv[1]);
  // int numThread = argc > 1 ? atoi(argv[1]) : 2;

  // int size = atoi(argv[2]);
  // string operation = argv[3];

  if (numThread == 0)
  {
    cerr << "Error: Number of threads cannot be zero.\n";
    return -1;
  }

  int rc = user_main(argc, argv);
  return rc;
}

#define main user_main