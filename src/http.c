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

    // handle SIGPIPE here.
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

    thread_node_t* tnode = (thread_node_t*) arg;
    route = tnode->route;
    client = tnode->client;

    if (client.err != 0) {
        printf("client error");
        if (client.fd > 0) conn_close(client);
        if (arg) free(arg);
        return;
    }

    string_t BUF = new_n_string(BUF_SIZE);
    if (BUF.data == NULL) {
        // error
        printf("[%d] BUF failed\n", client.fd);
        if (send(client.fd, CLOSE_CONN, strlen(CLOSE_CONN), 0) < 0) {
            printf("[%d] CLOSE_CONN also. faield\n", client.fd);
        }
        conn_close(client);
        if (arg) free(arg);
        return;
    }

    // set send, recv timeouts
    struct timeval send_time_out = {.tv_sec = temp_server->send_timeout, .tv_usec = 0};
    if(setsockopt(client.fd, SOL_SOCKET, SO_SNDTIMEO, &send_time_out, sizeof(send_time_out)) < 0) {
        printf("set send timeout error\n");
        return;
    }

    struct timeval recv_time_out = {.tv_sec = temp_server->recv_timeout, .tv_usec = 0};
    if (setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, &recv_time_out, sizeof(recv_time_out)) < 0) {
        printf("set recv timeout error\n");
        return;
    }

    request_ctx_t ctx;
    init_ctx(&ctx);

    // client is ipv4
    struct sockaddr_in *temp_client = (struct sockaddr_in*) &client.addr;
    char clinet_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &temp_client, clinet_ip, INET_ADDRSTRLEN);

    printf("[%d][%s:%d] Conn handling\n", client.fd, clinet_ip, ntohs(temp_client->sin_port));

    while(1) {
        // receive http request
        int n = recv(client.fd, BUF.data, BUF_SIZE - 1, 0);
        if (n == 0 || temp_server->shut_down) break;
        if (n < 0) {
            // if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) goto RESET_CTX;
            // else break;
            
            // client disconnects, close the connection, but what to do.? does browser need to know if connection is closed ?
            break;
        }
        BUF.data[n] = '\0';

        // parse http request
        int ok = parse_http_request(&ctx, BUF.data, n);
        if (ok == 1) {
            // 400 Bad Request
            printf("400 Bad Request\n");
            goto RESET_CTX;
        } else {
            // printf("PATH = %s, LEN = %d\n", ctx.req.path, ctx.req.path_len);
            // logs client addr..
            // printf("[%s] = %s\n", client_ip, ctx.req.path);

        }

        // check if connection is closed or keep-alive
        if (is_closed_conn(&ctx)) {
            printf("[%d] disconnected\n", client.fd);
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

    // Handle SIGPIPE here..
    // int close_len = send(client.fd, CLOSE_CONN, strlen(CLOSE_CONN), 0);

    printf("[%d][%s:%d] closed\n", client.fd, clinet_ip, ntohs(temp_client->sin_port));

    conn_close(client);
    if (arg) free(arg);
    if (BUF.data) free_string(BUF);
}

int serve_and_listen(server_t* server, const char *address) {
    listener_t ln = s_listen("tcp", address);
    if (ln.err != 0) {
        perror("listerner failed");
        return 0;
    }

    while(atomic_load(&server->shut_down) == false) {
        client_t conn = s_accept(ln);
        if (conn.err == 0) {
            thread_node_t* tnode = (thread_node_t*) malloc(sizeof(thread_node_t));
            if (tnode == NULL) {
                conn_close(conn);
                continue;
            }
            tnode->client = conn;
            tnode->route = &server->route; 
            push_task(server->pool, handle_conn, (void*) tnode);
        } else {
            break;
        }
    }

    if (temp_server->shut_down == false) {
        s_close(ln);
    }
    destroy_route(&server->route);
    destroy_pool(server->pool, MAX_THREAD_COUNT);
    return 0;
}

// Signal handling CTRL + C
static void shut_down_server(int sig) {
    if (sig == SIGINT) {
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

    server->recv_timeout = 10;
    server->send_timeout = 10;

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