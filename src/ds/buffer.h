#ifndef BUFFER_H
#define BUFFER_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

// string_t stores string array and it's len.
// appending will force allocation
// data is stored at heap
// can be freed using free_string()
// for any error, check if data=null
typedef struct String {
    int len;
    char* data;
} string_t;

// new_string() takes a c_string, allocates n sizeof memory in heap and copies c_string
// check str.data = NULL, for failure
string_t new_string(char* str);

void reset_string(string_t* str);

// copies str to another string_t
// str is not freed, for any error, check if data = null
string_t copy_string(string_t str);

// allocates len sized memeory at heap
string_t new_n_string(int len);

// creates an empty string, len = 0
void init_string(string_t* s);

// append b into a, frees string b
// returns false if a = null or b = null or malloc() fails
bool append_string(string_t* a, string_t* b);

// append b into a, dosen't free b (you are responsible)
// retrun false if a = null, b = nul or malloc() fails
bool append_string_cstr(string_t* a, char* b);

// frees data allocation
void free_string(string_t s);

#endif