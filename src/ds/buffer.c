#include"buffer.h"

string_t new_string(char* str) {
    // printf("new_string() str=%s\n", str);
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
// b frees
bool append_string(string_t* a, string_t* b) {
    if (a == NULL || b == NULL) return false;
    if (a->data == NULL || b->data == NULL) return false;
    // what happens here ? we store the pointer in another variable, waht if realloc fails ?? ptr will be null, but a->data ?
    char* ptr = (char*) realloc(a->data, sizeof(char) * (a->len + b->len + 1));
    if (ptr == NULL) {
        return false;
    }
    strncpy(ptr + a->len, b->data, sizeof(char) * b->len);
    ptr[a->len + b->len] = '\0';
    a->data = ptr;
    a->len += b->len;
    // printf("append_string() str=%s\n", a->data);
    free_string(*b);
    return true;
}

bool append_string_cstr(string_t* a, char* b) {
    if (a == NULL || b == NULL) return false;
    if (a->data == NULL) return false;
    int b_len = strlen(b);
    char* ptr = (char*) realloc(a->data, sizeof(char) * (a->len + b_len + 1));
    if (ptr == NULL) {
        return false;
    }
    strncpy(ptr + a->len, b, sizeof(char) * b_len);
    ptr[a->len + b_len] = '\0';
    a->data = ptr;
    a->len += b_len;
    // printf("append_string() str=%s\n", a->data);
    return true;
}

void free_string(string_t s) {
    if (s.data != NULL) free(s.data);
}