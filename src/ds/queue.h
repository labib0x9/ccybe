#ifndef QUEUE_H
#define QUEUE_H

#include<stdlib.h>
#include<stdbool.h>

// function pointer to store function...
// function return type = void
// function argumrnt type = void*
typedef void (* func) (void* args);

// for storing a function and it's arguments.
typedef struct task_node {
    func func;
    void *args;
} task_node_t;

// queue_t acts as a FIFO / queue
// if len = cap, it resizes by 2 * cap
// but cap can be maximum 1e6, by design
// queue only stores task_node_t, not generics
typedef struct {
   task_node_t* q;
   int cap, len;
   int readAt, insertAt;
} queue_t;

// allocates memory at heap to store cap sized task_node_t struct
// can be freed by free_queue(), free_queue_heap()
// returns false if q is null or malloc fails.
bool init_queue(queue_t* q, int cap);

// check if full
// can give wrong ans if q = NULL, this case returns always false.
bool is_full(queue_t* q);

// chcek if empty
// can give wrong ans if q = NULL, this case returns always false.
bool is_empty(queue_t* q);

// push at the back of the queue,
// if queue is full (len = cap), then it resizes by 2 * len.
// return false if resizes fails or q is empty
bool push(queue_t* q, task_node_t val);

// pop from the front of the queue
// value is stored to val, to work on poped value.
// also if you dont need the value, just pass NULL
// return false if q is empty or null
bool pop(queue_t* q, task_node_t* val);

// free queue for stack allocation (queue_t)
// frees the internal array of task_not_t
void free_queue(queue_t* q);

// free queue for heap allocation (queue_t)
// frees the internal array of task_not_t, also the queue_t 
void free_queue_heap(queue_t* q);

#endif