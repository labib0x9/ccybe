#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<stdio.h>
#include<pthread.h>
#include<stdbool.h>
#include"queue.h"

#define MAX_THREAD_COUNT 4

// *** creates 4 threads to work concurrentl ***
// uses mutex and condition variables to perform locking, waiting and wake mechanism.
// allocations are controlled via destroy_pool()
typedef struct {
    pthread_t thread[MAX_THREAD_COUNT];
    pthread_mutex_t lock;
    pthread_cond_t cond;
    queue_t* q;
    bool shutdown;
} thread_pool_t;

// creates a threadpool of MAX_THREAD_SIZE threads and queueSize sized queue to store tasks
// initializes queue, mutex and condition variables.
// thread_pool and queue are heap stored..
// queue is dynamic.
thread_pool_t* create_pool(int queueSize);

// this is the worker thread. it will take a task from the queue and execute it, 
// until the execution is done, this thread is no avaliable for further work.
// worker waits if the queue is empty and shutdown is not signaled.
// worker only stops when the shutdown signal is given.
void* start_pool(void* tPool);

// push a task to thread pool queue, if shutdown is not occured.
// signal the sleeping threads to wakeup
// returns true on successful push
// arg is allocated on heap, so ownership is lies with the creator.
// *** In case of future change, do not allocate memory here. ***
bool push_task(thread_pool_t* pool, void (* func) (void* args), void* arg);

// destroy pool with threadCount thread
// destroys mutex, cond variable
// frees pool and queue
// wakeups all the slepping threads.
void destroy_pool(thread_pool_t* pool, int threadCount);

#endif