#ifndef QUEUE_H
#define QUEUE_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<stdbool.h>

// task_node_t stores a function and function's parameter
typedef struct task_node {
    void (* func) (void* args);
    void *args;
} task_node_t;

// queue_t acts as a FIFO
// if len = cap, it resizes by 2 * cap
// but cap can be maximum 1e6. by design
// queue only stores task_node_t, not generics 
typedef struct {
   task_node_t* q;
   int cap, len;
   int readAt, insertAt;
} queue_t;

// initialize the queue
bool init_queue(queue_t* q, int cap);

// check if full
bool is_full(queue_t* q);

// chcek if empty
bool is_empty(queue_t* q);

// push value, append to end
bool push(queue_t* q, task_node_t val);

// pop value, remove the first one value
// value is stored to val, to work on poped value.
bool pop(queue_t* q, task_node_t* val);

// free queue
void free_queue(queue_t* q);

#endif