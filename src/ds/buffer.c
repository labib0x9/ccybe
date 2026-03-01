#include"buffer.h"

string_t new_string(char* str) {
    string_t tmp = {.len = 0, .data = NULL};
    if (str == NULL) return tmp;

    int len = strlen(str);
    char* data = (char*) malloc(sizeof(char) * (len + 1));
    if (data == NULL) return tmp;

    tmp.len = len;
    tmp.data = data;
    // #ifdef __APPLE__
    //     strncpy(tmp.data, str, sizeof(char) * len);
    // #elif defined(__linux__)
    //     memcpy(tmp.data, str, len);
    // #endif
    strcpy(tmp.data, str);
    tmp.data[len] = '\0';
    
    return tmp;
}

string_t new_n_string(int len) {
    string_t tmp = {.len = 0, .data = NULL};

    char* data = (char*) malloc(sizeof(char) * (len + 1));
    if (data == NULL) {
        printf("[log] new_n_string()=malloc() failed\n");
        return tmp;
    }
    
    tmp.data = data;
    tmp.len = len;
    tmp.data[len] = '\0';
    return tmp;
}

void init_string(string_t* s) {
    if (s == NULL) return;
    *s = new_string("");
}

bool append_string(string_t* a, string_t* b) {
    if (a == NULL || b == NULL) return false;
    if (a->data == NULL || b->data == NULL) return false;
    
    // what happens here ? we store the pointer in another variable, waht if realloc fails ?? ptr will be null, but a->data ?
    char* ptr = (char*) realloc(a->data, sizeof(char) * (a->len + b->len + 1));
    if (ptr == NULL) return false;
    
    // #ifdef __APPLE__
    //     strncpy(ptr + a->len, b->data, sizeof(char) * b->len);
    // #elif defined(__linux__)
    //     memcpy(ptr + a->len, b, b->len);
    // #endif

    strcpy(ptr + a->len, b->data);
    ptr[a->len + b->len] = '\0';
    a->data = ptr;
    a->len += b->len;
    free_string(*b);
    
    return true;
}

bool append_string_cstr(string_t* a, char* b) {
    if (a == NULL || b == NULL) return false;
    if (a->data == NULL) return false;

    int b_len = strlen(b);
    char* ptr = (char*) realloc(a->data, sizeof(char) * (a->len + b_len + 1));
    if (ptr == NULL) return false;

    // #ifdef __APPLE__
    //     strncpy(ptr + a->len, b, sizeof(char) * b_len);
    // #elif defined(__linux__)
    //     memcpy(ptr + a->len, b, b_len);
    // #endif
    
    strcpy(ptr + a->len, b);
    ptr[a->len + b_len] = '\0';
    a->data = ptr;
    a->len += b_len;

    return true;
}

string_t copy_string(string_t str) {
    string_t new_str = new_n_string(str.len);
    if (new_str.data == NULL) return new_str;

    memcpy(new_str.data, str.data, str.len);
    return new_str;
}

void reset_string(string_t* str) {
    memset(str->data, 0, sizeof(str->len));
    str->len = 0;
}

void free_string(string_t s) {
    if (s.data != NULL) free(s.data);
}