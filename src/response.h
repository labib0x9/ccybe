#ifndef RESPONSE_H
#define RESPONSE_H

#include<stdio.h>
#include<stdbool.h>
#include"cnet.h"
#include"parser.h"
#include"khash.h"
#include"ds/buffer.h"
#include"route.h"

static const char path_template[] = 
    "HTTP/1.1 %d %s\r\n" 
    "Content-Length: %d\r\n"
    "Content-Type: text/plain\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "%s";

typedef struct Response {
    int status_code;
    char* status;
    string_t body;
} response_t;

string_t generate_response(response_t resp);

#endif