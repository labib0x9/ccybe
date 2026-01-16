#ifndef BUFFER_H
#define BUFFER_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct String {
    int len;
    char* data;
} string_t;

string_t new_string(char* str);
void init_string(string_t* s);
bool append_string(string_t a, string_t b);
void free_string(string_t s);

#endif