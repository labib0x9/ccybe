#ifndef PARSER_H
#define PARSER_H

#include<stdio.h>
#include"cnet.h"
#include<llhttp.h>
#include"khash.h"

// stores headers as key-value
typedef struct {
    char key[128];
    char value[128];
} header_t;

// stores parsed request
// multiple value in single key, stores as one entity. 
typedef struct Request {
    char method[8];
    char path[256];
    int path_len;
    header_t headers[128];
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

#endif