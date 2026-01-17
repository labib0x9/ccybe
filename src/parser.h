#ifndef PARSER_H
#define PARSER_H

#include<stdio.h>
#include"cnet.h"
#include<llhttp.h>
#include"khash.h"

#define MAX_KEY_SIZE 128
#define MAX_VALUE_SIZE 128
#define MAX_METHOD_SIZE 8
#define MAX_PATH_SIZE 256
#define MAX_HEADER_COUNT 128
#define MAX_QUERY_COUNT 10

enum {
    GET = 1,
    POST,
    PUT,
    DELETE,
    OPTION,
};

// stores data as key-value
typedef struct {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} pair_t;

// stores headers as key-value
typedef struct {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} header_t;

// stores path and parameter
typedef struct {
    char path[MAX_PATH_SIZE];
    pair_t queries[MAX_QUERY_COUNT];
} url_t;

// stores parsed request
// multiple value in single key, stores as one entity. 
typedef struct Request {
    char method[MAX_METHOD_SIZE];
    char path[MAX_PATH_SIZE];
    int path_len;
    header_t headers[MAX_HEADER_COUNT];
    int header_count;
    header_t cur_header;
    char* body; // heap allocated
} http_request_t;

// Combined. all in one context.
typedef struct {
    http_request_t req;
    llhttp_t parser;
    llhttp_settings_t settings;
} request_ctx_t;

void init_ctx(request_ctx_t* ctx);
void reset_ctx(request_ctx_t* ctx);
int parse_http_request(request_ctx_t* ctx, const char* buf, int n);

int get_mothod_type(const char* method);
bool is_closed_conn(request_ctx_t* ctx);

#endif