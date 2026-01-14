#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<stdbool.h>

// task node..
typedef struct task_node {
    void (* func) (void* args);
    void *args;
} task_node;

typedef struct {
   task_node* q;
   int cap;
   int len;
   int readAt, insertAt;

} queue;

bool init_queue(queue* q, int cap) {
    if (cap <= 0 || cap >= (int) 1e6) { // for now, no long capacity
        cap = 1;
    }
    q->cap = cap;
    q->len = 0;
    q->readAt = q->insertAt = 0;
    task_node *qq = (task_node*) malloc(sizeof(task_node) * q->cap);
    if (qq == NULL) {
        return false;
    }
    q->q = qq;
    return true;
}

bool isFull(queue* q) {
    return q->len == q->cap;
}

bool isEmpty(queue* q) {
    return q->len == 0;
}

bool resize(queue* q) {
    if (!isFull(q)) {
        return false;
    }
    int newCap = 2 * q->cap;
    if (newCap <= 0) {   // greater than 1e9
        return false;
    }
    task_node* newQ = (task_node*) malloc(sizeof(task_node) * newCap);
    if (newQ == NULL) {
        return false;
    }
    for (int i = 0; i < q->len; i++) {
        newQ[i] = q->q[(q->readAt + i) % q->cap];
    }
    free(q->q);
    q->q = newQ;
    q->cap = newCap;
    q->insertAt = q->len;
    q->readAt = 0;
    return true;
}

bool push(queue* q, task_node val) {
    if (isFull(q)) {
        bool ok = resize(q);
        if (!ok) return ok;
    }
    q->q[q->insertAt] = val;
    q->insertAt = (q->insertAt + 1) % q->cap; 
    q->len++;
    return true;
}

bool pop(queue* q, task_node* val) {
    if(isEmpty(q)) {
        return false;
    }
    *val = q->q[q->readAt];
    q->readAt = (q->readAt + 1) % q->cap;
    q->len--;
    return true;
}

void free_queue(queue* q) {
    if(q == NULL) return;
    if(q->q != NULL) free(q->q);
    if(q != NULL) free(q);
}