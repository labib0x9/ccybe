#include<stdio.h>
#include<stdbool.h>
#include"cnet.h"
#include"parser.h"
#include"khash.h"
#include"ds/buffer.h"
#include"route.h"

static const int BUF_SIZE = 2560;

char BUF[BUF_SIZE];

// For closing connection response.
static const char CLOSE_CONN[] =
    "HTTP/1.1 200 OK\r\n" 
    "Content-Length: 6\r\n"
    "Connection: close\r\n"
    "\r\n"
    "CLOSED";

static const char path_template[] = 
    "HTTP/1.1 %d %s\r\n" 
    "Content-Length: %d\r\n"
    "Content-Type: text/plain\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "%s";

enum {
    GET = 1,
    POST,
    PUT,
    DELETE,
    OPTION,
};

typedef struct Response {
    int status_code;
    char* status;
    string_t body;
} response_t;

// need to chcek for errors.
string_t generate_response(response_t resp) {
    int cap = resp.body.len + 128;
    char tmp[cap];

    int n = snprintf(tmp, cap, path_template, resp.status_code, resp.status, resp.body.len, resp.body.data);
    if (n < 0) {
        printf("Failed to generate resp\n");
        // continue;
        // 500 server internal
    }
    tmp[n] = '\0';

    printf("RESP = %s\n", tmp);
    return new_string(tmp);
}

void default_page(client_t* client, request_ctx_t* ctx) {
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

void api_front_page(client_t* client, request_ctx_t* ctx) {
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

// status codes are from llhttp library
void not_found_page(client_t* client, request_ctx_t* ctx) {
    // (void) client;
    // (void) ctx;

    response_t resp = {
        .status_code = HTTP_STATUS_NOT_FOUND,
        .status = "OK",
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

// method types
int get_mothod_type(const char* method) {
    if (strcasecmp("get", method) == 0) return GET;
    if (strcasecmp("post", method) == 0) return POST; 
    return -1;
}

// Connection: close
bool is_closed_conn(request_ctx_t* ctx) {
    for (int i = 0; i < ctx->req.header_count; i++) {
        if (strcasecmp("Connection", ctx->req.headers[i].key) == 0) {
            printf("Connection = %s\n", ctx->req.headers[i].value);
        }
        if (strcasecmp("Connection", ctx->req.headers[i].key) == 0 && strcasecmp("Close", ctx->req.headers[i].value) == 0) {
            return true;
        }
    }
    return false;
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
            continue;
        }
        BUF[n] = '\0';

        // parse http request
        int ok = parse_http_request(&ctx, BUF, n);
        if (ok == 1) {
            // 400 Bad Request
            printf("400 Bad Request\n");
            // reset_ctx(&ctx);
            goto RESET_CTX;
            continue;
        } else {
            printf("PATH = %s , LEN = %d\n", ctx.req.path, ctx.req.path_len);
        }

        // check if connection is closed or keep-alive
        if (is_closed_conn(&ctx)) {
            break;
        }

        // Method based response generate
        switch(get_mothod_type(ctx.req.method)) {
            case GET: {
                // search path in hash table
                khiter_t found = kh_get(route_map, route->route, ctx.req.path);
                if (found == kh_end(route->route)) {
                    // not found
                    printf("path not found = %s\n", ctx.req.path);
                    not_found_page(&client, &ctx);
                } else {
                    // printf("Value found = %d\n", kh_value(route->route, found));
                    printf("found path = %s\n", ctx.req.path);
                    route_handler_t handler = kh_value(route->route, found);
                    handler.func(&client, &ctx);
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

int serve_and_listen(route_t* route, const char *address) {
    listener_t ln = s_listen("tcp", address);
    if (ln.err != 0) {
        perror("listerner failed");
        // exit(1);
        return 0;
    }

    while(1) {
        client_t conn = s_accept(ln);
        handle_conn(route, conn);
    }

    s_close(ln);
}

int main() {

    route_t route;
    if (init_route(&route) != 0) {
        return 0;
    }

    route_register(&route, "/", default_page);
    route_register(&route, "/api", api_front_page);

    serve_and_listen(&route, ":8080");

    destroy_route(&route);

    return 0;
}

