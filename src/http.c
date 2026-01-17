#include"http.h"

// status codes are from llhttp library
void not_found_page(client_t* client, request_ctx_t* ctx) {
    response_t resp = {
        .status_code = HTTP_STATUS_NOT_FOUND,
        .status = "Not Found",
        .body = new_string("Path=")
    };

    if (append_string_cstr(&resp.body, ctx->req.path) == false) {
        printf("path append failed\n");
    }
    if (append_string_cstr(&resp.body, " not found") == false) {
        printf("nf append failed\n");
    }

    string_t raw_resp = generate_response(resp);

    // Send the respinse
    int n = send(client->fd, raw_resp.data, raw_resp.len, 0);
    if (n < 0) {
        printf("Failed to respond\n");
    }

    free_string(raw_resp);
    free_string(resp.body);
}

// if this path exists in www file.
void handle_not_found(client_t* client, request_ctx_t* ctx) {
    not_found_page(client, ctx);
}

// handle client function
// need to EAGAIN, EINTR
// need to set timeout
void handle_conn(route_t *route, client_t client) {

    request_ctx_t ctx;
    init_ctx(&ctx);

    while(1) {
        // receive http request
        int n = recv(client.fd, BUF, BUF_SIZE - 1, 0);
        if (n < 0) {
            goto RESET_CTX;
            // continue;
        }
        BUF[n] = '\0';

        // parse http request
        int ok = parse_http_request(&ctx, BUF, n);
        if (ok == 1) {
            // 400 Bad Request
            printf("400 Bad Request\n");
            // reset_ctx(&ctx);
            goto RESET_CTX;
            // continue;
        } else {
            printf("PATH = %s, LEN = %d\n", ctx.req.path, ctx.req.path_len);
        }

        // check if connection is closed or keep-alive
        if (is_closed_conn(&ctx)) {
            break;
        }

        // Method based response generate
        switch(get_mothod_type(ctx.req.method)) {
            case GET: {
                // search path in hash table
                route_handler_t handler;
                bool found = route_lookup(&handler, route, ctx.req.path);
                if (found) {
                    handler.func(&client, &ctx);
                } else {
                    handle_not_found(&client, &ctx);
                }
                break;
            }
            default: {
                printf("unknown method = %s\n", ctx.req.method);
                // send unknown method type
            }
        }
        
        RESET_CTX:
            reset_ctx(&ctx);
    }

    // Close the connection
    send(client.fd, CLOSE_CONN, strlen(CLOSE_CONN), 0);
    conn_close(client);
}

int serve_and_listen(server_t* server, const char *address) {
    listener_t ln = s_listen("tcp", address);
    if (ln.err != 0) {
        perror("listerner failed");
        return 0;
    }

    while(1) {
        client_t conn = s_accept(ln);
        handle_conn(&server->route, conn);
    }

    s_close(ln);
    destroy_route(&server->route);
    return 0;
}

void init_server(server_t* server) {
    route_t route;
    if (init_route(&route) != 0) {
        // error
        return;
    }
    server->route = route;
}

void register_route(server_t* server, const char* path, route_handler_fn func) {
    route_register(&server->route, path, func);
}