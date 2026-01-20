# clitocybe

A minimal HTTP server framework written in C, focused on learning low-level networking, request parsing, and server architecture.

---

## Overview

`clitocybe` provides a thin abstraction over POSIX sockets and HTTP parsing to help you build a basic HTTP/1.1 server without hiding core system concepts. It is **not** a production-ready framework; it is a learning-oriented project.

---

## Features

* TCP server abstraction (`socket`, `bind`, `listen`, `accept`)
* Basic HTTP/1.1 request parsing (via `llhttp`)
* Simple routing system (hash table based)
* Thread pool–based client handling
* Basic keep-alive support

---

## Usage

### 1. Create a TCP Listener

Create a TCP server bound to all interfaces on port `8080`. Internally, `s_listen()` wraps `socket()`, `bind()`, and `listen()`.

```c
listener_t ln = s_listen("tcp", ":8080");
if (ln.err != 0) {
    // handle error
}
```

---

### 2. Accept Client Connections

Accept incoming connections and abstract the file descriptor and address into a `client_t`.

```c
while (1) {
    client_t conn = s_accept(ln);
    // handle conn
}
```

---

### 3. Basic TCP Server Example

```c
#include "cnet.h"

void handle_conn(client_t client) {
    // handle client
    conn_close(client);
}

int main(void) {
    listener_t ln = s_listen("tcp", ":8080");
    if (ln.err != 0) {
        perror("listener failed");
        return 1;
    }

    while (1) {
        client_t conn = s_accept(ln);
        handle_conn(conn);
    }

    s_close(ln);
    return 0;
}
```

---

### 4. HTTP Parser Usage

Parse an HTTP/1.1 request using the built-in request context abstraction.

```c
#include "parser.h"

static const char CLOSE_CONN[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 6\r\n"
    "Connection: close\r\n"
    "\r\n"
    "CLOSED";

int main(void) {
    request_ctx_t ctx;
    init_ctx(&ctx);

    int ok = parse_http_request(&ctx, CLOSE_CONN, strlen(CLOSE_CONN));
    if (ok == 1) {
        // bad request
    } else {
        // process request
    }

    printf("Path = %s\n", ctx.req.path);

    reset_ctx(&ctx);
    return 0;
}
```

---

### 5. Basic HTTP Server Example

```c
#include "http.h"

void default_page(client_t *client, request_ctx_t *ctx) {
    // handle root path
}

void api_front_page(client_t *client, request_ctx_t *ctx) {
    // handle /api
}

int main(void) {
    server_t server;
    init_server(&server);

    register_route(&server, "/", default_page);
    register_route(&server, "/api", api_front_page);

    serve_and_listen(&server, ":8080");
    return 0;
}
```

## Useful APIs

```c
int serve_and_listen(server_t *server, const char *address);
void init_server(server_t *server);
void register_route(server_t *server, const char *path, route_handler_fn func);
typedef void (*route_handler_fn)(client_t *, request_ctx_t *);
```

---

## Build And Run

Run the provided Makefile:

```bash
make run
```

---

## Dependencies

* **llhttp** (git submodule) — HTTP request parsing
* **klib** (git submodule) — hash table implementation

---

## Current State

* Multiple client handling via a thread pool
* Client connections time out after ~5 seconds
* Blocking I/O model (`recv()` based)
* HTTP/1.1 request parsing (no chunked transfer encoding)
* Basic `GET` method support
* URL is not split into path and query string
* Multiple header values are not fully supported
* Basic `Connection: keep-alive` handling
* Routing via hash table lookup
* Graceful shutdown is incomplete: `SIGINT` does not stop worker threads because `recv()` blocks

---

## Roadmap

Planned improvements for a more complete HTTP/1.1 server:

* Static file serving
* Structured logging
* Robust error handling
* Proper connection timeouts
* Non-blocking I/O or timeout-based shutdown
* Proper signal handling and graceful termination

---

## Disclaimer

This project is intended for educational purposes. Expect breaking changes, rough edges, and incomplete features.
