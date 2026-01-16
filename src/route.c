#include"route.h"

int init_route(route_t* route) {
    route->route = kh_init(route_map);
    if (route->route == NULL) { 
        printf("route hash table failed\n");
        return 1;
    }
    return 0;
}

// keys (path) are allocated on heap, must be freed.
int route_register(route_t* route, const char* path, route_handler_fn func) {
    route_handler_t handler = {.func = func};
    // // (void) path;
    // (void) handler;
    char *key = strdup(path);   // allocate to heap.
    int ret;
    khiter_t it = kh_put(route_map, route->route, key, &ret);
    if (ret == -1) {
        return 1;   // allocation failed
    } else if (ret == 0) {
        return 2;   // route exits
    }

    // store the path
    kh_value(route->route, it) = handler; 

    return 0;
}

int destroy_route(route_t* route) {
    // free keys
    for (khiter_t k = kh_begin(route->route); k != kh_end(route->route); k++) {
        if (kh_exist(route->route, k)) {
            free((char*) kh_key(route->route, k));
        }
    }
    // destroy hash table
    kh_destroy(route_map, route->route);
    return 0;
}
