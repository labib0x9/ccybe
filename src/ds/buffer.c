#include"buffer.h"

// typedef struct String {
//     char *data;
//     int len, cap;
// } string_t;

// void init_string(string_t* s) {
//     memset(s, 0, sizeof(*s));
//     s->cap = 15;
//     char* buf = calloc(s->cap, s->cap);
//     if (buf == NULL) {
//         // error
//         s->cap = 0;
//     }
//     s->data = buf;
// }

string_t new_string(char* str) {
    string_t tmp = {.len = 0, .data = NULL};
    if (str == NULL) {
        return tmp;
    }
    int len = strlen(str);
    char* data = (char*) malloc(sizeof(char) * (len + 1));
    if (data == NULL) {
        return tmp;
    }
    tmp.len = len;
    tmp.data = data;
    strncpy(tmp.data, str, sizeof(char) * len);
    tmp.data[len] = '\0';
    return tmp;
}

void init_string(string_t* s) {
    if (s == NULL) return;
    *s = new_string("");
}

// a = a + b
bool append_string(string_t *a, string_t* b) {
    if (a == NULL || b == NULL) return false;
    if (a->data == NULL || b->data == NULL) return false;
    // what happens here ? we store the pointer in another variable, waht if realloc fails ?? ptr will be null, but a->data ?
    char *ptr = (char*) realloc(a->data, sizeof(char) * (a->len + b->len + 1));
    if (ptr == NULL) {
        return false;
    }
    strncpy(ptr + a->len, b->data, sizeof(char) * b->len);
    ptr[a->len + b->len] = '\0';
    a->data = ptr;
    a->len += b->len;
    free_string(*b);
    return true;
}

void free_string(string_t s) {
    if (s.data) free(s.data);
}