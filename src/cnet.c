#include"cnet.h"

int get_protocol_type(const char* protocol) {
    if (strcasecmp(protocol, "tcp") == 0) return PROTOCOL_TCP;
    if (strcasecmp(protocol, "udp") == 0) return PROTOCOL_UDP;
    return -1;
}

// Seperate IPv4, return 0 on success
// "127.0.0.1:8080"
// ":8080"
// "255.255.255.255:65536"
// no invalid address or port handling right now.
int split_addr_port(addr_t* addr, const char* address) {
    if (!addr) return -1;
    if (!address) return -1;

    // seperate by ':'
    char* colonAt = strchr(address, ':');
    if (!colonAt) return -1;

    // address
    strncpy(addr->host, address, colonAt - address);
    addr->host[colonAt - address] = '\0';

    // port
    addr->port = atoi(colonAt + 1);
    return 0;
}

// need to implement SO_RCVTIMEO
void tcp_create_and_listen_ipv4(listener_t* ln) {
    // create socket
    ln->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ln->fd < 0) {
        ln->err = 2;
        return;
    }

    // configure socket
    int opt = 1;
    if (setsockopt(ln->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsocket error = SO_REUSEADDR");
        ln->err = 3;
        return;
    }
    if (setsockopt(ln->fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) == -1) {
        perror("setcocket error = SO_NOSIGPIPE");
        ln->err = 3;
        return;
    }
    // setsockopt(ln->fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    // setsockopt(ln->fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

    struct timeval tv = {.tv_sec = 5, .tv_usec = 0};
    setsockopt(ln->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // setup listening address
    // memset(&(ln->s_addr), 0, sizeof(ln->s_addr));
    // ln->s_addr.ss_family = AF_INET;
    // ln->s_addr.
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    memset(&addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ln->addr.port);
    if (ln->addr.host[0] == '\0') {
        addr.sin_addr.s_addr = INADDR_ANY;
        // printf("Any address\n");
    } else {
        if (inet_pton(AF_INET, ln->addr.host, &addr.sin_addr) == -1) {
            perror("inet_pton error");
            ln->err = 4;
            return;
        }
    }
    memcpy(&(ln->s_addr), &addr, addr_len);

    // bind
    if (bind(ln->fd, (struct sockaddr*) &(ln->s_addr), addr_len) == -1) {
        perror("bind error");
        ln->err = 5;
        return;
    }

    // listen
    if (listen(ln->fd, BACKLOG) == -1) {
        perror("listen error");
        ln->err = 6;
        return;
    }
}

// We will use only IPv4 and TCP only.
// network is TCP.
// err = 0, then no error
// err = 1, unknown protocol
// err = 2, socket creation
listener_t s_listen(const char* network, const char* address) {
    listener_t ln;
    ln.err = 0;
    ln.fd = 0;
    ln.err = split_addr_port(&(ln.addr), address);
    if (ln.err != 0) return ln;

    switch (get_protocol_type(network)) {
    case PROTOCOL_TCP:
        tcp_create_and_listen_ipv4(&ln);
        break;
    case PROTOCOL_UDP:
        break;
    default: {
            ln.err = 1;
            // return ln;
        }
    }

    return ln;
}

void s_close(listener_t ln) {
    close(ln.fd);
}

// listen on listener and accept incoming connection, store the address and return one client.
// SAccept(ln) -> return one client
// err ->
// 0 - > passed
// 1 -> accept error
client_t s_accept(listener_t ln) {
    client_t conn;
    conn.err = 0;
    socklen_t len = sizeof(conn.addr);
    printf("Listening...\n");
    conn.fd = accept(ln.fd, (struct sockaddr*) &conn.addr, &len);
    if (conn.fd < 0) {
        conn.err = 1;
        return conn;
    }
    printf("Accepted... %d\n", conn.fd);

    return conn;
}

int s_read(client_t client, char* b) {
    (void) client;
    (void) b;
    return 0;
}

int s_write(client_t client, char* b) {
    (void) client;
    (void) b;
    return 0;
}

// close the client fd
void conn_close(client_t client) {
    close(client.fd);
}