// // #include<stdio.h>
// // #include<string.h>
// // #include "server.h"

// // void registration() {
// //     // Register_Route("/health", "GET", health_handler);
// // }

// // void launch(Server* server) {
// //     printf("Waiting\n");
// //     int new_socket = accept(server->server_fd, (struct sockaddr *) &(server->address), sizeof(server->address));
// //     char buffer[3000];
// //     int n = read(new_socket, buffer, sizeof(buffer) - 1);
// //     buffer[n] = '\0';
// //     printf("%s\n", buffer);


// //     const char *HTTP_200_OK =
// //     "HTTP/1.1 200 OK\r\n"
// //     "Content-Type: text/html\r\n"
// //     "Content-Length: %d\r\n"
// //     "Connection: Closed\r\n"
// //     "\r\n"
// //     "<html><body><h1>HELLO FROM SERVER = WORLD</h1></body></body>";

// //     write(new_socket, HTTP_200_OK, strlen(HTTP_200_OK));
// //     close(new_socket);

// //     printf("Done\n");
// // }

// // int main() {

// //     // registration();

// //     // int err = listen_and_server("127.0.0.1:8080");
// //     // if (err == -1) {
// //     //     return 1;
// //     // }

// //     Server server = server_construction(AF_INET, SOCK_STREAM, 0, 8080, 10, INADDR_ANY, launch);
// //     server.launch(&server);

// //     return 0;
// // }

#include<stdio.h>
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
    "Content-Length: 6"
    "Connection: close\r\n"
    "\r\n"
    "CLOSED";

// handle client function
void handleConn(client_t client) {

    request_ctx_t ctx;
    init_ctx(&ctx);

    int n = recv(client.fd, BUF, BUF_SIZE - 1, 0);
    BUF[n] = '\0';

    int ok = parse_http_request(&ctx, BUF, n);
    if (ok == 1) {
        // 400 Bad Request
    } else {
        printf("PATH = %s\n", ctx.req.path);
    }

    // Close the connection
    send(client.fd, CLOSE_CONN, strlen(CLOSE_CONN), 0);
    ConnClose(client);
}

int ServerAndListen(const char *address) {
    listener_t ln = SListen("tcp", address);
    if (ln.err != 0) {
        perror("listerner failed");
        // exit(1);
        return 0;
    }

    while(1) {
        client_t conn = SAccept(ln);
        handleConn(conn);
    }

    SClose(ln);
}

int main() {

    ServerAndListen(":8080");

    return 0;
}
