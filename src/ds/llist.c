#include"llist.h"

list_t init_list() {
    list_t list = {
        .head = NULL,
        .tail = NULL,
    };
    return list;
}

// insert client at the back, O(1) push_back
// if fails return 1, else 0
l_node_t* insert_client(list_t* list, client_t client) {
    l_node_t* node = (l_node_t*)malloc(sizeof(l_node_t));
    memset(node, 0, sizeof(l_node_t));
    if (node == NULL) {
        // error 
        perror("Node malloc");
        return NULL;
    }

    node->client = client;
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail)
        list->tail->next = node;
    list->tail = node;

    if (!list->head)
        list->head = node;

    return node;
}

// search via file descriptor, fd
// TC: O(n)
l_node_t* search_client(list_t* list, int fd) {
    for (l_node_t* node = list->head; node != NULL; node = node->next) {
        if (node->client.fd == fd) {
            return node;
        }
    }
    return NULL;
}

// delete via file descriptor, fd
// TC: O(n)
// fails on return 1, else 0
int remove_client_fd(list_t* list, int fd) {
    l_node_t* node = search_client(list, fd);
    if (node == NULL) return 1;
    if (node->prev) node->prev->next = node->next;
    else list->head = node->next;
    if (node->next) node->next->prev = node->prev;
    else list->tail = node->prev;
    free(node);
    return 0;
}

int remove_client_node(list_t* list, l_node_t* node) {
    if (node == NULL) return 1;
    if (node->prev) node->prev->next = node->next;
    else list->head = node->next;
    if (node->next) node->next->prev = node->prev;
    else list->tail = node->prev;
    free(node);
    return 0;
}

// // delete via l_node_t
// // TC: O(1)
// void delete(list_t* list, l_node_t* node) {}
// client fd are closed here
void free_list(list_t* list) {
    while (list->head != NULL) {
        l_node_t* node = list->head;
        list->head = list->head->next;
        if (node != NULL) {
            close(node->client.fd);
            free(node);
        }
    } 
}