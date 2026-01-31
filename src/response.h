#ifndef RESPONSE_H
#define RESPONSE_H

#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include"cnet.h"
#include"parser.h"
#include"khash.h"
#include"ds/buffer.h"
#include"route.h"
#include"mime.h"
#include<fcntl.h>
#include<sys/stat.h>

static const char path_template[] = 
    "HTTP/1.1 %d %s\r\n" 
    "Content-Length: %d\r\n"
    "Content-Type: text/plain\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "%s";

typedef struct Response {
    int status_code;
    header_t headers[MAX_HEADER_COUNT];
    int header_count;
    string_t body;
    int err;
} http_response_t;

typedef struct ResponseCTX {
    http_response_t resp;
    client_t conn;
} response_ctx_t;

string_t generate_response(response_ctx_t* ctx);
void init_resp_ctx(response_ctx_t* ctx);
void reset_resp_ctx(response_ctx_t* ctx);
int write_response(response_ctx_t* ctx);
void set_header(response_ctx_t* ctx, const char* header, const char* value);


void handle_not_found(response_ctx_t* wctx, request_ctx_t* rctx);
void handle_static_files(response_ctx_t* wctx, char* file_path, int file_size, request_ctx_t* rctx);
void not_found_page(response_ctx_t* wctx, request_ctx_t* rctx);
void send_close_resp(response_ctx_t* wctx, request_ctx_t* rctx);


#endif