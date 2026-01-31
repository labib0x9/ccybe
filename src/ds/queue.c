#include"queue.h"

bool init_queue(queue_t* q, int cap) {
    if (q == NULL) return false;

    // for now, no long capacity
    if (cap <= 0 || cap > (int) 1e6) {
        cap = 1;
    }
    
    q->cap = cap;
    q->len = 0;
    q->readAt = q->insertAt = 0;
    
    task_node_t *qq = (task_node_t*) malloc(sizeof(task_node_t) * q->cap);
    if (qq == NULL) return false;

    q->q = qq;
    return true;
}

bool is_full(queue_t* q) {
    if (q == NULL) return false;
    return q->len == q->cap;
}

bool is_empty(queue_t* q) {
    if (q == NULL) return false;
    return q->len == 0;
}

// O(N) iteration.
// slow but canbe faster
bool resize(queue_t* q) {
    if (q == NULL) return false;
    if (!is_full(q)) return false;

    // greater than 1e7
    long long newCap = 2ll * q->cap;
    if (newCap >= (int) 1e7) return false;

    task_node_t* newQ = (task_node_t*) malloc(sizeof(task_node_t) * (int) newCap);
    if (newQ == NULL) return false;

    for (int i = 0; i < q->len; i++) {
        newQ[i] = q->q[(q->readAt + i) % q->cap];
    }

    free(q->q);
    q->q = newQ;
    q->cap = (int) newCap;
    q->insertAt = q->len;
    q->readAt = 0;
    return true;
}

// insert at back, if no room is free, then resizes by 2 * len
bool push(queue_t* q, task_node_t val) {
    if (q == NULL) return false;
    
    if (is_full(q)) {
        bool ok = resize(q);
        if (!ok) return ok;
    }
    
    q->q[q->insertAt] = val;
    q->insertAt = (q->insertAt + 1) % q->cap; 
    q->len++;
    return true;
}

bool pop(queue_t* q, task_node_t* val) {
    if (q == NULL) return false;
    if (is_empty(q)) return false;

    if (val != NULL) {
        *val = q->q[q->readAt];
    }
    
    q->readAt = (q->readAt + 1) % q->cap;
    q->len--;
    return true;
}

void free_queue_heap(queue_t* q) {
    if (q == NULL) return;
    if (q->q != NULL) free(q->q);
    if (q != NULL) free(q);
}

void free_queue(queue_t* q) {
    if (q == NULL) return;
    if (q->q != NULL) free(q->q);
}