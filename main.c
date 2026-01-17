#include"http.h"

void home_page(client_t* client, request_ctx_t* ctx) {
    // generate response
    response_t resp = {
        .status_code = HTTP_STATUS_OK,
        .status = "OK",
        .body = new_string(ctx->req.path)
    };
    
    string_t raw_resp = generate_response(resp);

    // Send the respinse
    int n = send(client->fd, raw_resp.data, raw_resp.len, 0);
    if (n < 0) {
        printf("Failed to respond\n");
    }

    free_string(raw_resp);
    free_string(resp.body);
}

void api_home_page(client_t* client, request_ctx_t* ctx) {
    response_t resp = {
        .status_code = HTTP_STATUS_OK,
        .status = "OK",
        .body = new_string(ctx->req.path)
    };

    string_t raw_resp = generate_response(resp);

    // Send the respinse
    int n = send(client->fd, raw_resp.data, raw_resp.len, 0);
    if (n < 0) {
        printf("Failed to respond\n");
    }

    free_string(raw_resp);
    free_string(resp.body);
}

int main() {

    server_t server;
    init_server(&server);

    register_route(&server, "/", home_page);
    register_route(&server, "/api", api_home_page);

    // load_static_file();

    serve_and_listen(&server, ":8080");

    return 0;
}

