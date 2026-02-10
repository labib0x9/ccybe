#include"threadpool.h"

thread_pool_t* create_pool(int queueSize) {
    thread_pool_t* pool = (thread_pool_t*) malloc(sizeof(thread_pool_t));
    if (pool == NULL) return NULL;

    pool->shutdown = false;
    
    pool->q = (queue_t*) malloc(sizeof(queue_t));
    if (pool->q == NULL) goto FREE_POOL;
    
    if (!init_queue(pool->q, queueSize)) goto FREE_QUEUE;
    if (pthread_mutex_init(&pool->lock, NULL) != 0) goto FREE_QUEUE;
    if (pthread_cond_init(&pool->cond, NULL) != 0) goto DESTROY_MUTEX;

    for (int i = 0; i < MAX_THREAD_COUNT; i++) {
        if (pthread_create(&(pool->thread[i]), NULL, start_pool, (void*) pool) != 0) {
            destroy_pool(pool, i);
            return NULL;
        }
    }
    
    printf("[info] Threadpool is created for %d threads\n", MAX_THREAD_COUNT);
    return pool;

    // Cleanups on failure
    DESTROY_MUTEX:
        pthread_mutex_destroy(&pool->lock);
    FREE_QUEUE:
        free_queue_heap(pool->q);
    FREE_POOL:
        free(pool);
    return NULL;
}

void destroy_pool(thread_pool_t* pool, int threadCount) {
    if (pool == NULL) return;

    pthread_mutex_lock(&pool->lock);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->lock);
    
    for (int i = 0; i < threadCount; i++) {
        pthread_join(pool->thread[i], NULL);
    }

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);
    
    free_queue_heap(pool->q);
    free(pool);

    printf("[info] Threadpool is destrooyed\n");
}

bool push_task(thread_pool_t* pool, void (* func) (void* args), void* arg) {
    if (pool == NULL || func == NULL || arg == NULL) {
        return false;
    }

    pthread_mutex_lock(&pool->lock);
    if (pool->shutdown) goto NO_NEW_TASK;

    task_node_t node = {
        .func = func,
        .args = (void*) arg
    };

    if (!push(pool->q, node)) goto NO_NEW_TASK;

    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->lock);
    return true;

    // queue not accepting task
    NO_NEW_TASK:
        pthread_mutex_unlock(&pool->lock);
    return false;
}

void* start_pool(void* tPool) { 
    if (tPool == NULL) goto EXIT_WORKER;

    thread_pool_t* pool = (thread_pool_t*) (tPool);
    if (pool == NULL) goto EXIT_WORKER;

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
        
        bool ok = pop(pool->q, &task);
        pthread_mutex_unlock(&pool->lock);

        if (!ok) continue;
        
        // executing task...
        (*(task.func))(task.args);
    }

    EXIT_WORKER:
        pthread_exit(NULL);
    return NULL;
}