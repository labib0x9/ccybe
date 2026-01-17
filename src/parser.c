#include"parser.h"

// set all memory to 0.
void http_request_t_init(http_request_t* req) {
    memset(req, 0, sizeof(*req));
}

// Get the method
static int on_method(llhttp_t* parser, const char* at, size_t len) {
    http_request_t *r = parser->data;
    memcpy(r->method, at, len);
    r->method[len] = '\0';
    return 0;
}

// at = pointer from where path begins.
// length = path length
static int on_url(llhttp_t* parser, const char* at, size_t length) {
    http_request_t *r = parser->data;
    memcpy(r->path, at, length);
    r->path[length] = '\0';
    r->path_len = length;
    return 0;
}

// Gives the header fields only, store it in cur_header variable to access in on_header_value()
static int on_header_field(llhttp_t* parser, const char* at, size_t length) {
    http_request_t *r = parser->data;
    memcpy(r->cur_header.key, at, length);
    r->cur_header.key[length] = '\0';
    return 0;
}

// Gives the header values, parse it for multiple types seperated by ;
static int on_header_value(llhttp_t* parser, const char* at, size_t length) {
    http_request_t *r = parser->data;
    memcpy(r->cur_header.value, at, length);
    r->cur_header.value[length] = '\0';
    memcpy(&r->headers[r->header_count], &r->cur_header, sizeof(r->cur_header));
    r->header_count++;
    return 0;
}

static int on_header_complete(llhttp_t* parser) {
    http_request_t *r = parser->data;
    (void) r;
    return 0;
}

static int on_body(llhttp_t* parser, const char* at, size_t length) {
    http_request_t *r = parser->data;
    (void) r;
    (void) at;
    (void) length;
    return 0;
}

static int on_message_complete(llhttp_t* parser) {
    http_request_t *r = parser->data;
    (void) r;
    return 0;
}

// intitalize the context
void init_ctx(request_ctx_t* ctx) {
    http_request_t_init(&ctx->req);

     // initialize settings
    llhttp_settings_init(&ctx->settings);

    // defines callbacks on settings
    ctx->settings.on_url = on_url;
    ctx->settings.on_header_field = on_header_field;
    ctx->settings.on_header_value = on_header_value;
    ctx->settings.on_message_complete = on_message_complete;
    ctx->settings.on_body = on_body;
    ctx->settings.on_headers_complete = on_header_complete;
    ctx->settings.on_method = on_method;

    reset_ctx(ctx);
}

// Reset context for reuse.
void reset_ctx(request_ctx_t* ctx) {
    llhttp_init(&ctx->parser, HTTP_REQUEST, &ctx->settings);
    ctx->parser.data = &ctx->req;
}

// 1 = 400 Bad Request
// 0 = success
int parse_http_request(request_ctx_t* ctx, const char* buf, int n) {
    llhttp_errno_t err = llhttp_execute(&ctx->parser, buf, n);
    if (err != HPE_OK) return 1;
    return 0;
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