#ifndef CNET_H
#define CNET_H

/**  Need To Implement -> net.Listen("tcp", ":8080")  **/
/*
    SListen("tcp", ":8080")
    -> supports ipv4 and ipv6 in single implementation. (Only ipv4 for now)
    -> Return a ready-to-accept fd, abstract socket creation, options set, bound, listening.
    -> Expose zero protocol trivia to caller. no sockaddr_in, AF_INET.
    -> Return explicit error. Caller can inspect caused error.
    -> Returns listener_t
*/

#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>

typedef struct {
    int port;
    char host[24];
} addr_t;

typedef struct {
    struct sockaddr_storage s_addr;
    addr_t addr;
    int err;
    int fd;
} listener_t;

typedef struct {
    int fd;
    struct sockaddr_storage addr;
    int err;
} client_t;

static const int BACKLOG = 10;

enum {
    PROTOCOL_TCP,
    PROTOCOL_UDP,
};

enum {
    SCREATE_ERR,
};

listener_t s_listen(const char* network, const char* address);
void s_close(listener_t ln);
client_t s_accept(listener_t ln);
void conn_close(client_t client);
int s_read(client_t client, char* b);
int s_write(client_t client, char* b);

#endif