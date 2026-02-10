#ifndef ROUTE_H
#define ROUTE_H

#include<stdio.h>
#include<stdbool.h>
#include"cnet.h"
#include"parser.h"
#include"khash.h"
#include"ds/buffer.h"
#include"response.h"

typedef struct ResponseCTX response_ctx_t;
typedef struct RequestCTX request_ctx_t;

typedef void (*route_handler_fn)(response_ctx_t*, request_ctx_t*);

typedef struct Handler {
    route_handler_fn func;
} route_handler_t;

// to sotre paths.
KHASH_MAP_INIT_STR(route_map, route_handler_t);

typedef struct Route {
    khash_t(route_map) *route;
} route_t;

int init_route(route_t* route);
int route_register(route_t* route, const char* path, route_handler_fn func);
int destroy_route(route_t* route);
bool route_lookup(route_handler_t* handler, route_t *route, char* path);

#endif