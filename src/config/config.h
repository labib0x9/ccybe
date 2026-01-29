#ifndef CONFIG_H
#define CONFIG_H

#define MAX_KEY_SIZE 128
#define MAX_VALUE_SIZE 128
#define MAX_METHOD_SIZE 8
#define MAX_PATH_SIZE 256
#define MAX_HEADER_COUNT 128
#define MAX_QUERY_COUNT 10

enum {
    GET = 1,
    POST,
    PUT,
    DELETE,
    OPTION,
};


// stores data as key-value
typedef struct Pair {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} pair_t;


#endif