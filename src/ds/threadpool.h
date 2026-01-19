#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<stdbool.h>
#include"../cnet.h"
#include"../route.h"
#include"queue.h"

#define MAX_THREAD_COUNT 4

typedef struct thread_node {
    route_t *route;
    client_t client;
} thread_node_t;

// creates 4 thread to work concurrently
// uses mutex and locking to perform
typedef struct {
    pthread_t thread[MAX_THREAD_COUNT];
    pthread_mutex_t lock;
    pthread_cond_t cond;
    queue_t* q;
    bool shutdown;
} thread_pool_t;

// creates a thread pool, and you will initialize the queueSize, 
// queue is dynamic (Note)
thread_pool_t* create_pool(int queueSize);

// starts the pool
void* start_pool(void* tPool);

// push tasks to pool queue, 
// accepts function pointer and arguments.
bool push_task(thread_pool_t* pool, void (* func) (void* args), route_t* route, client_t client);

// destroy the pool
void destroy_pool(thread_pool_t* pool, int threadCount);

#endif