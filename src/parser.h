#ifndef PARSER_H
#define PARSER_H

#include<stdio.h>
#include<strings.h>
#include"cnet.h"
#include<llhttp.h>
#include"khash.h"
#include"url/url.h"
#include"url/decoder.h"
#include"config/config.h"

// stores headers as key-value
typedef struct {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} header_t;

// stores parsed request
// multiple value in single key, stores as one entity. 
typedef struct Request {
    char method[MAX_METHOD_SIZE];
    char raw_path[MAX_PATH_SIZE];
    int path_len;
    url_t url;
    header_t headers[MAX_HEADER_COUNT];
    int header_count;
    header_t cur_header;
    char* body; // heap allocated
} http_request_t;

// Combined. all in one context.
typedef struct RequestCTX {
    http_request_t req;
    llhttp_t parser;
    llhttp_settings_t settings;
} request_ctx_t;

void init_req_ctx(request_ctx_t* ctx);
void reset_req_ctx(request_ctx_t* ctx);
int parse_http_request(request_ctx_t* ctx, const char* buf, int n);

int get_mothod_type(const char* method);
bool is_closed_conn(request_ctx_t* ctx);

#endif