// #include<stdio.h>
// #include<string.h>
// #include "server.h"

// void registration() {
//     // Register_Route("/health", "GET", health_handler);
// }

// void launch(Server* server) {
//     printf("Waiting\n");
//     int new_socket = accept(server->server_fd, (struct sockaddr *) &(server->address), sizeof(server->address));
//     char buffer[3000];
//     int n = read(new_socket, buffer, sizeof(buffer) - 1);
//     buffer[n] = '\0';
//     printf("%s\n", buffer);


//     const char *HTTP_200_OK =
//     "HTTP/1.1 200 OK\r\n"
//     "Content-Type: text/html\r\n"
//     "Content-Length: %d\r\n"
//     "Connection: Closed\r\n"
//     "\r\n"
//     "<html><body><h1>HELLO FROM SERVER = WORLD</h1></body></body>";

//     write(new_socket, HTTP_200_OK, strlen(HTTP_200_OK));
//     close(new_socket);

//     printf("Done\n");
// }

// int main() {

//     // registration();

//     // int err = listen_and_server("127.0.0.1:8080");
//     // if (err == -1) {
//     //     return 1;
//     // }

//     Server server = server_construction(AF_INET, SOCK_STREAM, 0, 8080, 10, INADDR_ANY, launch);
//     server.launch(&server);

//     return 0;
// }

#include<stdio.h>
#include"cnet.h"
#include<llhttp.h>
#include"khash.h"

static const int BUF_SIZE = 2560;

typedef struct String {
    char *data;
    int len, cap;
} string_t;

typedef struct {
    char data[BUF_SIZE];
    // int insert_at, read_at;
} buffer_t;

typedef struct {
    char method[8];
    char path[512];
    // hashmap headers;
    char *body;
} http_request_t;

static const char CLOSE_CONN[] =
    "HTTP/1.1 200 OK\r\n"
    "Connection: close\r\n"
    "\r\n";

// handle client function
void handleConn(client_t client) {
    buffer_t req;
    int n = recv(client.fd, req.data, BUF_SIZE - 1, 0);
    req.data[n] = '\0';

    printf("%d : %s\n", n, req.data);

    // Close the connection
    send(client.fd, CLOSE_CONN, strlen(CLOSE_CONN), 0);
    ConnClose(client);
}

int main() {

    listener_t ln = SListen("tcp", ":8080");
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

    return 0;
}