#ifndef LLIST_H
#define LLIST_H

#include"../cnet.h"

typedef struct LinkedListNode {
    client_t client;
    struct LinkedListNode* prev;
    struct LinkedListNode* next;
} l_node_t;

typedef struct LinkedList {
    l_node_t* head;
    l_node_t* tail;
} list_t;

list_t init_list();
l_node_t* insert_client(list_t* list, client_t client);
l_node_t* search_client(list_t* list, int fd);
int remove_client_fd(list_t* list, int fd);
int remove_client_node(list_t* list, l_node_t* node);
void free_list(list_t* list);

#endif
