#include<stdio.h>
#include<stdbool.h>
#include"cnet.h"
#include"parser.h"

static const int BUF_SIZE = 2560;

typedef struct String {
    char *data;
    int len, cap;
} string_t;

char BUF[BUF_SIZE];

// For closing connection response.
static const char CLOSE_CONN[] =
    "HTTP/1.1 200 OK\r\n" 
    "Content-Length: 6\r\n"
    "Connection: close\r\n"
    "\r\n"
    "CLOSED";

static const char path_template[] = 
    "HTTP/1.1 200 OK\r\n" 
    "Content-Length: %d\r\n"
    "Content-Type: text/plain\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "%s";

// handle client function
// need to EAGAIN, EINTR
void handle_conn(client_t client) {

    request_ctx_t ctx;
    init_ctx(&ctx);

    while(1) {
        int n = recv(client.fd, BUF, BUF_SIZE - 1, 0);
        if (n < 0) {
            continue;
        }
        BUF[n] = '\0';

        int ok = parse_http_request(&ctx, BUF, n);
        if (ok == 1) {
            // 400 Bad Request
            printf("400 Bad Request\n");
            reset_ctx(&ctx);
            continue;
        } else {
            printf("PATH = %s , LEN = %d\n", ctx.req.path, ctx.req.path_len);
        }

        bool conn_closed = false;
        for (int i = 0; i < ctx.req.header_count; i++) {
            if (strcasecmp("Connection", ctx.req.headers[i].key) == 0) {
                printf("Connection = %s\n", ctx.req.headers[i].value);
            }
            if (strcasecmp("Connection", ctx.req.headers[i].key) == 0 && strcasecmp("Close", ctx.req.headers[i].value) == 0) {
                conn_closed = true;
            }
        }

        if (conn_closed) break;

        // Send the Path
        int cap = sizeof(path_template) + ctx.req.path_len + 1;
        char resp[cap];
        n = snprintf(resp, cap, path_template, ctx.req.path_len, ctx.req.path);
        if (n < 0) {
            printf("Failed to generate resp\n");
            continue;
        }
        resp[n] = '\0';

        printf("RESP = %s\n", resp);
        
        n = send(client.fd, resp, n, 0);
        if (n < 0) {
            printf("Failed to respond\n");
        }
        
        reset_ctx(&ctx);
    }

    // Close the connection
    send(client.fd, CLOSE_CONN, strlen(CLOSE_CONN), 0);
    conn_close(client);
}

int serve_and_listen(const char *address) {
    listener_t ln = s_listen("tcp", address);
    if (ln.err != 0) {
        perror("listerner failed");
        // exit(1);
        return 0;
    }

    while(1) {
        client_t conn = s_accept(ln);
        handle_conn(conn);
    }

    s_close(ln);
}

int main() {

    serve_and_listen(":8080");

    return 0;
}
