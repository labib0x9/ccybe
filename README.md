# clitocybe
---
A simple server framework.
---
# Usage
---
- Creating a tcp server binding to all interfaces at port 8080. SListen() abstract socket(), bind(), listen()
```c
listener_t ln = s_listen("tcp", ":8080");
if (ln.err != 0) {
    // error
}
```
- Accept clients and astract the fd, address to client_t
```c
while(1) {
    client_t conn = s_accept(ln);
    // handle conn
}
```
- Run the makefile
```bash
```

---
- Basic tcp server
```c
#include"cnet.h"

void handle_conn(client_t client) {
    // 
    conn_close(client);
}

int main() {
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
```

- https parser usage
```c
#include"parser.h"

static const char CLOSE_CONN[] =
    "HTTP/1.1 200 OK\r\n" 
    "Content-Length: 6\r\n"
    "Connection: close\r\n"
    "\r\n"
    "CLOSED";

int main() {
    request_ctx_t ctx;
    init_ctx(&ctx);

    int ok = parse_http_request(&ctx, CLOSE_CONN, strlen(CLOSE_CONN));
    if (ok == 1) {
        // bad request
    } else {
        // procced request
    }

    printf("Path = %s\n", ctx.req.path);

    reset_ctx(&ctx);
}
```

---
# Dependencies :
---
- llhttp (git submodule) : for parsing http requests
- klib (git submodule) : for hashing

---
# Feature:
---
HTTP/1.1 web server
- Thread Pool
- Socket Programming
- Keep-Alive Connection handle
- HTTP request parser
- Static file serving
- Routing
- Logger
- Error handling