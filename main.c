#include"http.h"

void home_page_test(response_ctx_t* wctx, request_ctx_t* rctx) {
    (void) wctx;
    (void) rctx;
    set_header(wctx, "X-Hackerone-Id", "0xfaisal");

    // set_body(wctx, rctx->req.path);
}


void home_page(response_ctx_t* wctx, request_ctx_t* rctx) {
    (void) wctx;
    (void) rctx;
    // // generate response
    // response_t resp = {
    //     .status_code = HTTP_STATUS_OK,
    //     .status = "OK",
    //     .body = new_string(ctx->req.path)
    // };
    
    // string_t raw_resp = generate_response(resp);

    // // Send the respinse
    // int n = send(client->fd, raw_resp.data, raw_resp.len, 0);
    // if (n < 0) {
    //     printf("Failed to respond\n");
    // }

    // free_string(raw_resp);
    // free_string(resp.body);
}

void api_home_page(response_ctx_t* wctx, request_ctx_t* rctx) {
    (void) wctx;
    (void) rctx;

    // response_t resp = {
    //     .status_code = HTTP_STATUS_OK,
    //     .status = "OK",
    //     .body = new_string(ctx->req.path)
    // };

    // string_t raw_resp = generate_response(resp);

    // // Send the respinse
    // int n = send(client->fd, raw_resp.data, raw_resp.len, 0);
    // if (n < 0) {
    //     printf("Failed to respond\n");
    // }

    // free_string(raw_resp);
    // free_string(resp.body);
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

