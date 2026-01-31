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






void send_close_resp(response_ctx_t* wctx, request_ctx_t* rctx) {
    (void) wctx;
    (void) rctx;

    wctx->resp.status_code = HTTP_STATUS_OK;
    string_t temp = new_string("closed");
    wctx->resp.body = copy_string(temp);

    char length[33];
    snprintf(length, sizeof(length), "%d", wctx->resp.body.len);

    set_header(wctx, "Content-Length", length);
    set_header(wctx, "Content-Type", "text/plain");
    // set_header(wctx, "Connection", "keep-alive");
    set_header(wctx, "Connection", "close");
}

// status codes are from llhttp library
// 404 not found 
void not_found_page(response_ctx_t* wctx, request_ctx_t* rctx) {
    (void) wctx;
    (void) rctx;

    wctx->resp.status_code = HTTP_STATUS_NOT_FOUND;
    // wctx->resp.status = "Not Found";
    string_t temp = new_string("Path=");
    // wctx->resp.body = new_string("Path=");

    if (append_string_cstr(&temp, rctx->req.raw_path) == false) {
        perror("1 path append");
        // printf("path append failed\n");
    }
    if (append_string_cstr(&temp, " not found") == false) {
        perror("2 nf path append");
        // printf("nf append failed\n");
    }

    // "Content-Length: %d\r\n"
    // "Content-Type: text/plain\r\n"
    // "Connection: keep-alive\r\n"

    wctx->resp.body = copy_string(temp);

    char length[33];
    snprintf(length, sizeof(length), "%d", wctx->resp.body.len);

    set_header(wctx, "Content-Length", length);
    set_header(wctx, "Content-Type", "text/plain");
    // set_header(wctx, "Connection", "keep-alive");
    set_header(wctx, "Connection", "keep-alive");
}

// serves static files..
void handle_static_files(response_ctx_t* wctx, char* file_path, int file_size, request_ctx_t* rctx) {
    (void) wctx;
    (void) rctx;

    int mime_id = get_mime(file_path);
    if (mime_id == -1) {
        printf("Unsupported mime\n");
        return;
    }

    int file_fd = open(file_path, O_RDONLY, 0);
    if (file_fd < 0) {
        perror("Open file");
        return;
    }

    wctx->resp.status_code = HTTP_STATUS_OK;
    string_t temp = new_n_string(file_size + 1);

    int read_size = read(file_fd, temp.data, temp.len);
    if (read_size < 0) {
        free_string(temp);
        perror("Read file");
        return;
    }

    temp.len = read_size;

    wctx->resp.body = copy_string(temp);

    char length[33];
    snprintf(length, sizeof(length), "%d", wctx->resp.body.len);

    set_header(wctx, "Content-Length", length);

    switch (mime_id) {
        case JS: {
            set_header(wctx, "Content-Type", "application/javascript");
            break;
        }
        case HTML: {
            set_header(wctx, "Content-Type", "text/html");
            break;
        }
        case PNG: {
            set_header(wctx, "Content-Type", "image/png");
            break;
        }
        case CSS: {
            set_header(wctx, "Content-Type", "text/css");
            break;
        }
    }

    set_header(wctx, "Connection", "keep-alive");
}

// if this path exists in www file.
void handle_not_found(response_ctx_t* wctx, request_ctx_t* rctx) {
    // printf("PATH [NOT_FOUND]= %s\n", rctx->req.url.path);
    string_t temp_path = new_string("./www");
    if (append_string_cstr(&temp_path, rctx->req.url.path) == false) {
        // internal server error
        perror("handle_not_found: append");
        free_string(temp_path);
        return;
    }
    // printf("PATH [NOT_FOUND]= %s\n", temp_path.data);

    // if exists, then handle
    struct stat st;
    if (stat(temp_path.data, &st) < 0) {
        // 500 internal error
        perror("handle_not_found: stat");
        free_string(temp_path);
        return;
    }

    // 
    if (S_ISDIR(st.st_mode)) {
        // handle directory
    } else if (S_ISREG(st.st_mode)) {
        // handle regular file
        printf("[REG FILE] = %s\n", temp_path.data);
        
        // handle if user has permission, modification

        // else handle this
        handle_static_files(wctx, temp_path.data, st.st_size, rctx);
    } else {
         // else report 404 - not found
        not_found_page(wctx, rctx);
    }

    free_string(temp_path);
}