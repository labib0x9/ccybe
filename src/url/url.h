#ifndef URL_H
#define URL_H

#include<stdio.h>
#include"cnet.h"
#include<llhttp.h>
#include"khash.h"
#include"../config/config.h"
#include"decoder.h"

// stores path and parameter
typedef struct {
    char path[MAX_PATH_SIZE];
    pair_t queries[MAX_QUERY_COUNT];
    int query_count;
    int path_len;
} url_t;

int seperate_query(char* raw_path, url_t* url);

#endif