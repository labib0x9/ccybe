#include"response.h"

// need to chcek for errors.
string_t generate_response(response_t resp) {
    int cap = resp.body.len + 128;
    char tmp[cap];

    int n = snprintf(tmp, cap, path_template, resp.status_code, resp.status, resp.body.len, resp.body.data);
    if (n < 0) {
        printf("Failed to generate resp\n");
        // continue;
        // 500 server internal
    }
    tmp[n] = '\0';

    printf("RESP = %s\n", tmp);
    return new_string(tmp);
}