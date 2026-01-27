#include"response.h"

static const char first_line[] = "HTTP/1.1 %d %s\r\n";
static const char header_value[] = "%s: %s\r\n";
#define TEMP_LEN 216

char* get_status_code(int code) {
    switch(code) {
        case HTTP_STATUS_NOT_FOUND:
            return "Not Found";
        case HTTP_STATUS_OK:
            return "OK";
        case HTTP_STATUS_FORBIDDEN:
            return "Forbidden";
        case HTTP_STATUS_INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        default:
            return "Internal Server Error";
    }
    return NULL;
}

void init_resp_ctx(response_ctx_t* ctx) {
    (void) ctx;
}

void reset_resp_ctx(response_ctx_t* ctx) {
    (void) ctx;
}

// need to chcek for errors.
string_t generate_response(response_ctx_t* ctx) {
    // int cap = ctx->resp.body.len + 128;
    // string_t tmp = new_n_string(cap);

    // int n = snprintf(tmp.data, cap, path_template, ctx->resp.status_code, ctx->resp.status, ctx->resp.body.len, ctx->resp.body.data);
    // if (n < 0) {
    //     printf("0 - Failed to generate resp\n");
    //     // continue;
    //     // 500 server internal
    // }
    // tmp.data[n] = '\0';
    // tmp.len = n;

    string_t temp_resp = new_n_string(TEMP_LEN);
    int n = snprintf(temp_resp.data, TEMP_LEN, first_line, ctx->resp.status_code, get_status_code(ctx->resp.status_code));
    temp_resp.data[n] = '\0';
    temp_resp.len = n;

    string_t resp = new_string("");

    if (append_string(&resp, &temp_resp) == 0) {
        // error
        // 500 internal server error
        printf("1 - Failed to generate resp\n");
    }

    for (int i = 0; i < ctx->resp.header_count; i++) {
        string_t a = new_n_string(TEMP_LEN);
        n = snprintf(a.data, TEMP_LEN, header_value, ctx->resp.headers[i].key, ctx->resp.headers[i].value);
        a.data[n] = '\0';
        a.len = n;
        if (append_string(&resp, &a) == 0) {
            // error
            // 500 internal server error
            printf("2 - Failed to generate resp\n");
        }
    }

    string_t seperator = new_string("\r\n");
    if (append_string(&resp, &seperator) == 0) {
        // error
        // 500 internal server error
        printf("3 - Failed to generate resp\n");
    }

    string_t body = copy_string(ctx->resp.body);

    if (append_string(&resp, &body) == 0) {
        // error
        // 500 internal server error
        printf("4 - Failed to generate resp\n");
    }

    // printf("RESP = %s\n", tmp);
    return resp;
}

// this one frees response....
int write_response(response_ctx_t* ctx) {
    string_t resp = generate_response(ctx);

    printf("RESP= %s\n", resp.data);
    printf("RESP LEN= %d\n", resp.len);
    printf("SOCK= %d\n", ctx->conn.fd);

    // Send the respinse
    int n = send(ctx->conn.fd, resp.data, resp.len, 0);
    if (n < 0) {
        printf("SEND = Failed to respond\n");
        perror("SEND");
    }

    free_string(resp);
    free_string(ctx->resp.body);
    return 0;
}

// sets headers, overrides if value is present.
void set_header(response_ctx_t* ctx, const char* header, const char* value) {
    // ctx->resp.headers[ctx->resp.header_count].key = header;
    // memcpy(ctx->resp.headers[ctx->resp.header_count].key, header, strlen(header));
    // ctx->resp.headers[ctx->resp.header_count].key[strlen(header)] = '\0';

    // // ctx->resp.headers[ctx->resp.header_count].value = value;
    // memcpy(ctx->resp.headers[ctx->resp.header_count].value, value, strlen(value));
    // ctx->resp.headers[ctx->resp.header_count].key[strlen(value)] = '\0';

    header_t *h = &ctx->resp.headers[ctx->resp.header_count];
    (void) h;

    int klen = strlen(header);
    int vlen = strlen(value);

    // printf("%s %s  :: %d %d\n\n", header, value, klen, vlen);
    memcpy(h->key, header, klen);
    h->key[klen] = '\0';

    memcpy(h->value, value, vlen);
    h->value[vlen] = '\0';

    ctx->resp.header_count++;
}