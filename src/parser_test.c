// #include<stdio.h>
// #include"cnet.h"
// #include"parser.h"

// static const int BUF_SIZE = 2560;

// typedef struct String {
//     char *data;
//     int len, cap;
// } string_t;

// char BUF[BUF_SIZE];

// // // stores headers as key-value
// // typedef struct {
// //     char key[128];
// //     char value[128];
// // } header_t;

// // // stores parsed request
// // // multiple value in single key, stores as one entity. 
// // typedef struct Request {
// //     char method[8];
// //     char path[256];
// //     int path_len;
// //     header_t headers[128];
// //     int header_count;
// //     header_t cur_header;
// //     char* body; // heap allocated
// // } http_request_t;

// // For closing connection response.
// static const char CLOSE_CONN[] =
//     "HTTP/1.1 200 OK\r\n" 
//     "Content-Length: 6"
//     "Connection: close\r\n"
//     "\r\n"
//     "CLOSED";

// // // set all memory to 0.
// // void http_request_t_init(http_request_t* req) {
// //     memset(req, 0, sizeof(*req));
// // }

// // // Get the method
// // static int on_method(llhttp_t* parser, const char* at, size_t len) {
// //     http_request_t *r = parser->data;
// //     memcpy(r->method, at, len);
// //     r->method[len] = '\0';
// //     return 0;
// // }

// // // at = pointer from where path begins.
// // // length = path length
// // static int on_url(llhttp_t* parser, const char* at, size_t length) {
// //     http_request_t *r = parser->data;
// //     memcpy(r->path, at, length);
// //     r->path[length] = '\0';
// //     return 0;
// // }

// // // Gives the header fields only, store it in cur_header variable to access in on_header_value()
// // static int on_header_field(llhttp_t* parser, const char* at, size_t length) {
// //     http_request_t *r = parser->data;
// //     memcpy(r->cur_header.key, at, length);
// //     r->cur_header.key[length] = '\0';
// //     return 0;
// // }

// // // Gives the header values, parse it for multiple types seperated by ;
// // static int on_header_value(llhttp_t* parser, const char* at, size_t length) {
// //     http_request_t *r = parser->data;
// //     memcpy(r->cur_header.value, at, length);
// //     r->cur_header.value[length] = '\0';
// //     memcpy(&r->headers[r->header_count], &r->cur_header, sizeof(r->cur_header));
// //     r->header_count++;
// //     return 0;
// // }

// // static int on_header_complete(llhttp_t* parser) {
// //     http_request_t *r = parser->data;
// //     (void) r;
// //     return 0;
// // }

// // static int on_body(llhttp_t* parser, const char* at, size_t length) {
// //     http_request_t *r = parser->data;
// //     (void) r;
// //     (void) at;
// //     (void) length;
// //     return 0;
// // }

// // static int on_message_complete(llhttp_t* parser) {
// //     http_request_t *r = parser->data;
// //     (void) r;
// //     return 0;
// // }

// // void init_parser(http_request_t* req, llhttp_t* parser, llhttp_settings_t* settings) {
// //     // initialize parser with settings
// //     llhttp_init(parser, HTTP_REQUEST, settings);
// //     parser->data = req;
// // }

// // handle client function
// void handleConn(client_t client) {

//     // // request struct
//     // http_request_t req;
//     // http_request_t_init(&req);

//     // // http parser and settings
//     // llhttp_t parser;
//     // llhttp_settings_t settings;

//     // // initialize settings
//     // llhttp_settings_init(&settings);

//     // // defines callbacks on settings
//     // settings.on_url = on_url;
//     // settings.on_header_field = on_header_field;
//     // settings.on_header_value = on_header_value;
//     // settings.on_message_complete = on_message_complete;
//     // settings.on_body = on_body;
//     // settings.on_headers_complete = on_header_complete;
//     // settings.on_method = on_method;

//     // init_parser(&req, &parser, &settings);

//     request_ctx_t ctx;
//     init_ctx(&ctx);

//     int n = recv(client.fd, BUF, BUF_SIZE - 1, 0);
//     BUF[n] = '\0';

//     // // But, how can i store parsed req into http_request_t struct ???
//     // llhttp_errno_t err = llhttp_execute(&parser, BUF, n);

//     // // invalid input, you should close the connection.
//     // // 400 Bad Request
//     // if (err != HPE_OK) {
//     //     fprintf(stderr, "Parse error: %s\n", llhttp_errno_name(err));
//     // }

//     // if (parser.finish) {
//     //     printf("finished\n");
//     // }

//     int ok = parse_http_request(&ctx, BUF, n);
//     if (ok == 1) {
//         // 400 Bad Request
//     } else {
//         printf("PATH = %s\n", ctx.req.path);
//     }



//     // printf("PATH = %s\n", req.path);
//     // printf("2nd header KEY = %s\n", req.headers[1].key);
//     // printf("2nd header VALUE = %s\n", req.headers[1].value);
//     // printf("MATHOD = %s\n", req.method);

//     // Close the connection
//     send(client.fd, CLOSE_CONN, strlen(CLOSE_CONN), 0);
//     ConnClose(client);
// }

// int ServerAndListen(const char *address) {
//     listener_t ln = SListen("tcp", address);
//     if (ln.err != 0) {
//         perror("listerner failed");
//         // exit(1);
//         return 0;
//     }

//     while(1) {
//         client_t conn = SAccept(ln);
//         handleConn(conn);
//     }

//     SClose(ln);
// }

// int main() {

//     ServerAndListen(":8080");

//     return 0;
// }
