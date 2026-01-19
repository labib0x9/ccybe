#include"response.h"

// need to chcek for errors.
string_t generate_response(response_t resp) {
    int cap = resp.body.len + 128;
    string_t tmp = new_n_string(cap);

    int n = snprintf(tmp.data, cap, path_template, resp.status_code, resp.status, resp.body.len, resp.body.data);
    if (n < 0) {
        printf("Failed to generate resp\n");
        // continue;
        // 500 server internal
    }
    tmp.data[n] = '\0';
    tmp.len = n;

    // printf("RESP = %s\n", tmp);
    return tmp;
}