#include"threadpool.h"

// creates a threadpool of MAX_THREAD_SIZE threats.
// initialize mutex and condition variables.
thread_pool_t* create_pool(int queueSize) {
    thread_pool_t* pool = (thread_pool_t*) malloc(sizeof(thread_pool_t));
    if (pool == NULL) {
        return NULL;
    }
    pool->shutdown = false;
    pool->q = (queue_t*) malloc(sizeof(queue_t));
    if (pool->q == NULL) {
        free(pool);
        return NULL;
    }
    if (!init_queue(pool->q, queueSize)) {
        free_queue(pool->q);
        free(pool);
        return NULL;
    }
    if (pthread_mutex_init(&pool->lock, NULL) != 0) {
        free_queue(pool->q);
        free(pool);
        return NULL;
    }
    if (pthread_cond_init(&pool->cond, NULL) != 0) {
        pthread_mutex_destroy(&pool->lock);
        free_queue(pool->q);
        free(pool);
        return NULL;
    }
    for (int i = 0; i < MAX_THREAD_COUNT; i++) {
        if (pthread_create(&(pool->thread[i]), NULL, start_pool, (void*) pool) != 0) {
            destroy_pool(pool, i);
            return NULL;
        }
    }
    printf("Threadpool created for %d threads\n", MAX_THREAD_COUNT);
    return pool;
}

// push a task to thread poll
// signal the thread poll to wakeup
// heap allocation here.
bool push_task(thread_pool_t* pool, void (* func) (void* args), route_t* route, client_t client) {
    if (pool == NULL || func == NULL) {
        return false;
    }
    pthread_mutex_lock(&pool->lock);
    if (pool->shutdown) {
        pthread_mutex_unlock(&pool->lock);
        return false;
    }

    // allocate to heap
    // handle arguments..
    thread_node_t* tnode = (thread_node_t*) malloc(sizeof(thread_node_t));
    tnode->client = client;
    tnode->route = route; 

    task_node_t node = {
        .func = func,
        .args = (void*) tnode
    };

    if (!push(pool->q, node)) {
        pthread_mutex_unlock(&pool->lock);
        return false;
    }
    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->lock);
    return true;
}

// destroy pool
void destroy_pool(thread_pool_t* pool, int threadCount) {
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->lock);
    for (int i = 0; i < threadCount; i++) {
        pthread_join(pool->thread[i], NULL);
    }
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);
    free_queue(pool->q);
    free(pool);

    printf("Threadpool destrooyed\n");
}

// starts the pool
// waits until a task is pushed to pool if queue is empty
void* start_pool(void* tPool) {
    thread_pool_t* pool = (thread_pool_t*) (tPool);
    task_node_t task;
    while(1) {
        pthread_mutex_lock(&pool->lock);
        while(is_empty(pool->q) && !pool->shutdown) {
            pthread_cond_wait(&pool->cond, &pool->lock);
        }
        if (pool->shutdown && is_empty(pool->q)) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }
        pop(pool->q, &task);
        pthread_mutex_unlock(&pool->lock);
        // executing task...
        (*(task.func))(task.args);
    }
    pthread_exit(NULL);
    return NULL;
}