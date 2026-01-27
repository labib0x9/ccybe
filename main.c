#include"http.h"

void home_page_test(response_ctx_t* wctx, request_ctx_t* rctx) {
    (void) wctx;
    (void) rctx;

    // wctx->resp.status = "Not Found";
    string_t temp = new_string("Path=");
    // wctx->resp.body = new_string("Path=");

    if (append_string_cstr(&temp, rctx->req.path) == false) {
        perror("1 path append");
        // printf("path append failed\n");
    }

    // "Content-Length: %d\r\n"
    // "Content-Type: text/plain\r\n"
    // "Connection: keep-alive\r\n"

    wctx->resp.body = copy_string(temp);

    char length[33];
    snprintf(length, sizeof(length), "%d", wctx->resp.body.len);

    wctx->resp.status_code = HTTP_STATUS_OK;

    set_header(wctx, "Content-Length", length);
    set_header(wctx, "Content-Type", "text/plain");
    set_header(wctx, "X-Hackerone-Id", "0xfaisal");
    // set_header(wctx, "Connection", "keep-alive");
    set_header(wctx, "Connection", "close");

    // set_body(wctx, rctx->req.path);
}


void home_page(response_ctx_t* wctx, request_ctx_t* rctx) {
    string_t temp = new_string("Path=");

    if (append_string_cstr(&temp, rctx->req.path) == false) {
        perror("1 path append");
        wctx->resp.status_code = HTTP_STATUS_INTERNAL_SERVER_ERROR;
        goto WRITE_HEADER;
    }

    wctx->resp.body = copy_string(temp);

    wctx->resp.status_code = HTTP_STATUS_OK;

    char length[33];
    WRITE_HEADER:
        snprintf(length, sizeof(length), "%d", wctx->resp.body.len);

        set_header(wctx, "Content-Length", length);
        set_header(wctx, "Content-Type", "text/plain");
        set_header(wctx, "X-Hackerone-Id", "0xfaisal");
        set_header(wctx, "Connection", "close");
}

void api_home_page(response_ctx_t* wctx, request_ctx_t* rctx) {
    string_t temp = new_string("Path=");

    if (append_string_cstr(&temp, rctx->req.path) == false) {
        perror("1 path append");
        wctx->resp.status_code = HTTP_STATUS_INTERNAL_SERVER_ERROR;
        goto WRITE_HEADER;
    }

    wctx->resp.body = copy_string(temp);

    wctx->resp.status_code = HTTP_STATUS_OK;

    char length[33];
    WRITE_HEADER:
        snprintf(length, sizeof(length), "%d", wctx->resp.body.len);

        set_header(wctx, "Content-Length", length);
        set_header(wctx, "Content-Type", "text/plain");
        set_header(wctx, "X-Hackerone-Id", "0xfaisal");
        set_header(wctx, "Connection", "close");
}

int main() {

    server_t server;
    init_server(&server);
    
    // default timeouts are 10 sec for both
    // set custom timeout
    server.recv_timeout = 60;
    server.send_timeout = 60;

    register_route(&server, "/", home_page);
    register_route(&server, "/api", api_home_page);

    // load_static_file();

    serve_and_listen(&server, ":8080");

    return 0;
}

