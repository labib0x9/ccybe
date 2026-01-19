#include"http.h"
#include"ds/threadpool.h"

static const int QUEUE_SIZE = 128;
static server_t* temp_server = NULL;

// status codes are from llhttp library
// 404 not found 
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
// why void* (void*) ? to push into thread pool
// arg is heap allocated..
// void handle_conn(route_t *route, client_t client) {
void handle_conn(void* arg) {
    // int offset = 0;
    route_t* route;
    client_t client;

    // memcpy(&route, arg, sizeof(route));
    // offset += sizeof(route);

    // memcpy(&client, arg + offset, sizeof(client));

    thread_node_t* tnode = (thread_node_t*) arg;
    route = tnode->route;
    client = tnode->client;

    request_ctx_t ctx;
    init_ctx(&ctx);

    while(1) {
        // receive http request
        int n = recv(client.fd, BUF, BUF_SIZE - 1, 0);
        if (n == 0) break;
        if (n < 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) goto RESET_CTX;
            else break;
        }
        BUF[n] = '\0';

        // parse http request
        int ok = parse_http_request(&ctx, BUF, n);
        if (ok == 1) {
            // 400 Bad Request
            printf("400 Bad Request\n");
            goto RESET_CTX;
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

    // send(client.fd, CLOSE_CONN, strlen(CLOSE_CONN), 0);
    conn_close(client);
    if (arg) free(arg);
    // return NULL;
}

int serve_and_listen(server_t* server, const char *address) {
    listener_t ln = s_listen("tcp", address);
    if (ln.err != 0) {
        perror("listerner failed");
        return 0;
    }

    while(atomic_load(&server->shut_down) == false) {
        client_t conn = s_accept(ln);
        // handle_conn(&server->route, conn);
        push_task(server->pool, handle_conn, &server->route, conn);
    }

    // s_close(ln);
    destroy_route(&server->route);
    destroy_pool(server->pool, MAX_THREAD_COUNT);
    return 0;
}

// Signal handling CTRL + C
static void shut_down_server(int sig) {
	// server->shutdown_signal = 1;
	// close(server->ln.fd);
    (void) sig;
    if (sig == SIGINT) {
        // temp_server->shut_down;
        atomic_store(&temp_server->shut_down, true);
        close(temp_server->ln.fd);
    }
}

void init_server(server_t* server) {
    route_t route;
    if (init_route(&route) != 0) {
        // error
        return;
    }
    server->route = route;

    server->pool = create_pool(QUEUE_SIZE);
    if (server->pool == NULL) {
        // error
        return;
    }

    atomic_init(&server->shut_down, false);
    temp_server = server;

    // // handle ctrl + c
    // signal(SIGINT, shut_down_server);

    struct sigaction sa = {0};
    // sa.sa_flags = 0;
    sa.sa_handler = shut_down_server;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);
}

void register_route(server_t* server, const char* path, route_handler_fn func) {
    route_register(&server->route, path, func);
}