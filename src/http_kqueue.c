#ifdef __APPLE__

#include"http.h"
#include"ds/threadpool.h"
#include<sys/stat.h>
#include<sys/unistd.h>
#include<fcntl.h>

#include"mime.h"

static const int QUEUE_SIZE = 128;
static server_t* temp_server = NULL;
static char buffer[216];
pthread_mutex_t pipe_lock = PTHREAD_MUTEX_INITIALIZER;

int set_fd_nonblock(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0) return flag;
    return fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

static void shut_down_server(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        temp_server->shutdown_signal = true;
    }
}

// creates listening socket and make it non-blocking
// also, push listening socket to event queue
int init_listener(server_t* server, const char *address) {
    server->ln = s_listen("tcp", address);
    if (server->ln.err != 0) {
        // goto LISTENER_ERR;
        server->ln.err = 1;
        return 1;
    }

    if (set_fd_nonblock(server->ln.fd) < 0) {
        // goto SET_NON_BLOCK_ERR;
        server->ln.err = 2;
        return 2;
    }

    struct kevent ev;
    EV_SET(&ev, server->ln.fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
    if (kevent(server->efd, &ev, 1, NULL, 0, NULL) < 0) {
        // goto LISTENER_EVENT_ERR;
        server->ln.err = 3;
        return 3;
    }

    server->ln.err = 0;
    return 0;
}

void remove_client(server_t* server, int client_fd) {
    list_t* client_list = &server->client_list;
    l_node_t* node = search_client(client_list, client_fd);
    if (node == NULL) {
        return;
    }
    
    struct kevent ev[3];
    EV_SET(&ev[0], client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    // EV_SET(&ev[1], client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    kevent(server->efd, ev, 1, NULL, 0, NULL);

    conn_close(node->client);
    remove_client_node(client_list, node);
}

void handle_conn(void* arg) {
    if (arg == NULL) { 
        // should not be null....
    }
    thread_node_t* tnode = (thread_node_t*) arg;
    server_t* server = tnode->server;
    client_t client = tnode->client;
    string_t buffer = tnode->buffer;
    request_ctx_t* rctx = tnode->req;
    response_ctx_t* wctx = tnode->resp;

    if (buffer.data == NULL) {
        // buffer is null, handle allocation
        return;
    }

    init_req_ctx(rctx);
    init_resp_ctx(wctx);

    // parse http request
    int ok = parse_http_request(rctx, buffer.data, buffer.len);
    if (ok == 1) {
        // 400 Bad Request, handle allocation
        printf("400 Bad Request\n");
        return;
    }

    printf("[%s:%d]     %s      %s\n", client.c_addr.host, client.c_addr.port, rctx->req.method, rctx->req.raw_path);

    // Method based response generate
    switch(get_mothod_type(rctx->req.method)) {
        case GET: {
            // search path in hash table
            route_handler_t handler;
            bool found = route_lookup(&handler, &server->route, rctx->req.url.path);
            if (found) {
                handler.func(wctx, rctx);
            } else {
                handle_not_found(wctx, rctx);
            }
            break;
        }
        case POST: {
            break;
        }
        default: {
            printf("unknown method = %s\n", rctx->req.method);
            // send unknown method type
        }
    }

    tnode->resp->complete_resp = generate_response(wctx);

    struct kevent ev;
    EV_SET(&ev, client.fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, tnode);
    if (kevent(server->efd, &ev, 1, NULL, 0, NULL) < 0) {
        // what to do..:0)
    }
}

// accepts client connection, set client fd non-blocking
// push client fd to kqueue to read, read behaviour -> if buffer not drained, fire continuosly..:)
// list's node is allocated on heap, kqueue keeps the node..
void accept_client(server_t* server) {
    while (1) {
        client_t conn = s_accept(server->ln);
        if (conn.fd == -1) break;
        if (conn.err != 0) {
            conn_close(conn);
            continue;
        }

        struct sockaddr_in *temp_client = (struct sockaddr_in*) &conn.addr;
        inet_ntop(AF_INET, &temp_client->sin_addr, conn.c_addr.host, INET_ADDRSTRLEN);
        conn.c_addr.host[INET_ADDRSTRLEN] = '\0';
        conn.c_addr.port = ntohs(temp_client->sin_port);

        if (set_fd_nonblock(conn.fd) == -1) {
            conn_close(conn);
            continue;
        };

        printf("[%d][][%s:%d] Conn handling\n", conn.fd, conn.c_addr.host, conn.c_addr.port);

        l_node_t* node = insert_client(&server->client_list, conn);
        if (node == NULL) {
            conn_close(conn);
            printf("[%d][][%s:%d] Conn failed\n", conn.fd, conn.c_addr.host, conn.c_addr.port);
            continue;
        }

        struct kevent ev;
        EV_SET(&ev, conn.fd, EVFILT_READ, EV_ADD, 0, 0, node);
        if (kevent(server->efd, &ev, 1, NULL, 0, NULL) < 0) {
            printf("[%d][][%s:%d] Conn failed\n", conn.fd, conn.c_addr.host, conn.c_addr.port);
            conn_close(conn);
        }
    }
}

// BUF doesn't free here, But who owns BUF ?
// who owns tnode ??
// if client is null, then what ??
// read request and pass it to worker thread to parse, process and response generate.
void read_from_client(server_t* server, client_t* client) {
    if (client == NULL) {
        // search client
    }
    string_t BUF = new_string("");

    while (1) {
        int n = recv(client->fd, buffer, sizeof(buffer) - 1, 0);
        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            // BUF release.
            return;
        }
        if (n == 0) {
            printf("[%d][][%s:%d] Conn Disconnected\n", client->fd, client->c_addr.host, client->c_addr.port);
            remove_client(server, client->fd);
            return;
        }
        buffer[n] = '\0';
        int ok = append_string_cstr(&BUF, buffer);
        if (!ok) {
            // request parsing failed, handle allocation
            // BUF release or 500 internal error
            free_string(BUF);
            return;
        }
    }
    
    thread_node_t* tnode = (thread_node_t*) malloc(sizeof(thread_node_t));
    if (tnode == NULL) {
        // BUF release or 500 internal error
        free_string(BUF);
        return;
    }
    
    // BUF doesn't free here, But who owns BUF ?
    // who owns tnode ??
    // tnode->buffer is another allocation
    tnode->client = *client;
    tnode->server = server;

    tnode->buffer = copy_string(BUF);
    if (tnode->buffer.data == NULL) {
        free(tnode);
        free_string(BUF);
        return;
    }

    tnode->req = (request_ctx_t*) malloc(sizeof(request_ctx_t));
    if (tnode->req == NULL) {
        free_string(BUF);
        free_string(tnode->buffer);
        free(tnode);
        return;
    }

    tnode->resp = (response_ctx_t*) malloc(sizeof(response_ctx_t));
    if (tnode->resp == NULL) {
        free_string(BUF);
        free_string(tnode->buffer);
        free(tnode->req);
        free(tnode);
        return;
    }

    push_task(server->pool, handle_conn, (void*) tnode);

    free_string(BUF);
}

void write_to_client(int client_fd, thread_node_t* tnode) {
    (void) client_fd;   // not used, do not comment out..
    string_t resp = tnode->resp->complete_resp;
    client_t client = tnode->client;
    int total_send = 0;

    printf("[Log] WRITE RESPONSE\n");
    printf("[RESP] = %s\n", resp.data);
    printf("[RESP] len=%d\n", resp.len);

    while (total_send < resp.len) {
        int n = send(client.fd, resp.data + total_send, resp.len - total_send, 0);
        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                struct kevent ev;
                EV_SET(&ev, client.fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, tnode);
                kevent(tnode->server->efd, &ev, 1, NULL, 0, NULL);
                return;
            }
            // error, handle allocation
            remove_client(tnode->server, client.fd);
            return;
        }
        if (n == 0) {
            // error, handle allocation
            remove_client(tnode->server, client.fd);
            return;
        }
        total_send += n;
    }

    shutdown(tnode->client.fd, SHUT_WR);

    // no keep-alive handling
    remove_client(tnode->server, client.fd);

    free_string(tnode->buffer);
    free(tnode->resp);
    free(tnode->req);
    free(tnode);
}

// initliazes listener, set listener fd to non-block.
// kqueue initialize, and iterates over the events...
int serve_and_listen(server_t* server, const char *address) {
    if (init_listener(server, address) != 0) goto LISTENER_ERR;

    while (server->shutdown_signal == false) {
        int n = kevent(server->efd, NULL, 0, server->events, MAX_EVENT_SIZE, NULL);
        if (n == -1) {
            if (errno == EINTR) continue;
            // error
            break;
        }

        for (int i = 0; i < n; i++) {
            struct kevent* e = &server->events[i];
            int ident = e->ident;
            void* data = e->udata;
            int filter = e->filter;

            switch (filter) {
                case EVFILT_READ: {
                    if (e->flags & EV_EOF) {
                        remove_client(server, ident);
                        continue;
                    }
                    if (ident == server->ln.fd) {
                        accept_client(server);
                    } else {
                        l_node_t* node = (l_node_t*) data;
                        read_from_client(server, &node->client);
                    }
                    break;
                }
                case EVFILT_WRITE: {
                    write_to_client(ident, (thread_node_t*) data);
                    break;
                }
                case EVFILT_TIMER: {
                    // handle_client_timeout();
                    break;
                }
                default: {
                    // [info] filter not found..
                }
            }
        }
    }

    s_close(server->ln);
    // close(server->notify_fd[0]);
    // close(server->notify_fd[1]);
    destroy_route(&server->route);
    destroy_pool(server->pool, MAX_THREAD_COUNT);
    free_list(&server->client_list);
    return 0;

    // errors
    LISTENER_ERR:
        if (server->ln.err != 1) {
            s_close(server->ln);
        }
        // close(server->notify_fd[0]);
        // close(server->notify_fd[1]);
        destroy_route(&server->route);
        destroy_pool(server->pool, MAX_THREAD_COUNT);
        free_list(&server->client_list);
    return 1;
}

// initializes the server with default timeout 10sec.
// route, pool, signal handling, pipe, kqueue
// pipe[0] non-blocking..
// push pipe[0] to event queue..
void init_server(server_t* server) {
    temp_server = server;
    route_t route;
    if (init_route(&route) != 0) goto ROUTE_ERR;
    server->route = route;

    server->pool = create_pool(QUEUE_SIZE);
    if (server->pool == NULL) goto POOL_ERR;

    server->recv_timeout = 10;
    server->send_timeout = 10;
    server->shutdown_signal = false;

    struct sigaction sa = {0};
    // sa.sa_flags = 0;
    sa.sa_handler = shut_down_server;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) < 0) goto SIGACTION_ERR;
    if (sigaction(SIGTERM, &sa, NULL) < 0) goto SIGACTION_ERR;
    signal(SIGPIPE, SIG_IGN);

    // if (pipe(server->notify_fd) < 0) goto PIPE_SET_ERR;
    // if (set_fd_nonblock(server->notify_fd[0]) < 0) goto PIPE_NON_BLOCK_ERR;

    server->efd = kqueue();
    if (server->efd < 0) goto KQUEUE_ERR;

    // struct kevent kv;
    // EV_SET(&kv, server->notify_fd[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
    // if (kevent(server->efd, &kv, 1, NULL, 0, NULL) < 0) goto KQUEUE_PUSH_ERR;

    server->client_list = init_list();

    // success
    return;

    // KQUEUE_PUSH_ERR:
    KQUEUE_ERR:
    SIGACTION_ERR:
    POOL_ERR:
    ROUTE_ERR:
        return;

    // PIPE_NON_BLOCK_ERR:
    // PIPE_SET_ERR:
}

void register_route(server_t* server, const char* path, route_handler_fn func) {
    route_register(&server->route, path, func);
}

#endif