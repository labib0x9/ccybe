#pragma once
#ifndef HTTP_H
#define HTTP_H

#include<stdio.h>
#include<stdbool.h>
#include<stdatomic.h>
#include<signal.h>
#include"cnet.h"
#include"parser.h"
#include"khash.h"
#include"ds/buffer.h"
#include"route.h"
#include"response.h"
#include"ds/threadpool.h"
#include<errno.h>

static const int BUF_SIZE = 2560;

// For closing connection response.
static const char CLOSE_CONN[] =
    "HTTP/1.1 200 OK\r\n" 
    "Content-Length: 6\r\n"
    "Connection: close\r\n"
    "\r\n"
    "CLOSED";

typedef struct Server {
    listener_t ln;
    route_t route;
    thread_pool_t* pool;
    // volatile sig_atomic_t shutdown_signal;
    atomic_bool shut_down;
} server_t;

int serve_and_listen(server_t* server, const char *address);
void init_server(server_t* server);
void register_route(server_t* server, const char* path, route_handler_fn func);

#endif