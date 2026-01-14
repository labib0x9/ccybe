# clitocybe
---
A simple server framework.
---
# Usage
- Creating a tcp server binding to all interfaces at port 8080. SListen() abstract socket(), bind(), listen()
```c
listener_t ln = SListen("tcp", ":8080");
if (ln.err != 0) {
    // error
}
```
- Accept clients and astract the fd, address to client_t
```c
while(1) {
    client_t conn = SAccept(ln);
    // handle conn
}
```
- Run the makefile
```bash
```

---
Feature:
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