// int main() {
//     thread_pool* pool = create_pool(4);
//     if (pool == NULL) {
//         printf("Failed to create pool!\n");
//         return 1;
//     }

//     for (int i = 0; i < 10; i++) {
//         int* arg = malloc(sizeof(int));
//         *arg = i;
//         push_task(pool, print_task, arg);
//     }

//     //  sleep(5);
//     destroy_pool(pool, MAX_THREAD_COUNT);
//     printf("All tasks done, pool destroyed.\n");
//     return 0;
// }