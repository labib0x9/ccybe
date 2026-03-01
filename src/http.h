#pragma once
#ifndef HTTP_H
#define HTTP_H

#include<stdio.h>
#include<stdbool.h>
#include<stdatomic.h>
#include<signal.h>
#include<errno.h>

#include"cnet.h"
#include"parser.h"
#include"khash.h"
#include"route.h"
#include"response.h"
#include"ds/buffer.h"
#include"ds/threadpool.h"
#include"ds/llist.h"

#include"url/decoder.h"

#ifdef __APPLE__
    #include<sys/event.h>
    #include<sys/types.h>
#elif defined(__linux__)
    #include<sys/epoll.h>
#endif

static const int BUF_SIZE = 2560;
#define MAX_EVENT_SIZE 1024

// For closing connection response.
static const char CLOSE_CONN[] =
    "HTTP/1.1 200 OK\r\n" 
    "Content-Length: 6\r\n"
    "Connection: close\r\n"
    "\r\n"
    "CLOSED";

// default timeout is 10s.
typedef struct Server {
    listener_t ln;
    int efd;
    route_t route;
    thread_pool_t* pool;
    volatile sig_atomic_t shutdown_signal;
    time_t recv_timeout;
    time_t send_timeout;
    // int notify_fd[2];
    #ifdef __APPLE__
        struct kevent events[MAX_EVENT_SIZE];
    #elif defined(__linux__)
        struct epoll_event events[MAX_EVENT_SIZE];
    #endif
    list_t client_list;
} server_t;

typedef struct thread_node {
    server_t *server;
    client_t client;
    string_t buffer;
    response_ctx_t* resp;
    request_ctx_t* req;
} thread_node_t;

int serve_and_listen(server_t* server, const char *address);
void init_server(server_t* server);
void register_route(server_t* server, const char* path, route_handler_fn func);

#endif