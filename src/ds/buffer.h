#ifndef BUFFER_H
#define BUFFER_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

// string_t stores buffer into heap.
typedef struct String {
    int len;
    char* data;
} string_t;

// creates a string of str on heap
string_t new_string(char* str);

string_t copy_string(string_t str);

string_t new_n_string(int len);

// creates a empty string
void init_string(string_t* s);

// append b into a, frees string b
bool append_string(string_t* a, string_t* b);

// append b into a, dosen't free b (you are responsible)
bool append_string_cstr(string_t* a, char* b);

// free the string
void free_string(string_t s);

#endif