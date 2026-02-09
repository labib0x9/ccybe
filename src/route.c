#include"route.h"

int init_route(route_t* route) {
    route->route = kh_init(route_map);
    if (route->route == NULL) { 
        printf("route hash table failed\n");
        perror("route-init");
        return 1;
    }
    printf("Router initialized\n");
    return 0;
}

// keys (path) are allocated on heap, must be freed.
int route_register(route_t* route, const char* path, route_handler_fn func) {
    route_handler_t handler = {.func = func};

    char *key = strdup(path);   // allocate to heap.
    int ret;
    khiter_t it = kh_put(route_map, route->route, key, &ret);
    if (ret == -1) {
        perror("route register - alloc");
        return 1;   // allocation failed
    } else if (ret == 0) {
        perror("route exits");
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

bool route_lookup(route_handler_t* handler, route_t *route, char* path) {
    khiter_t found = kh_get(route_map, route->route, path);
    if (found == kh_end(route->route)) {
        // not found
        printf("path not found = %s\n", path);
        perror("route not found");
        return false;
    }
    
    printf("found path = %s\n", path);    
    *handler = kh_value(route->route, found);
    return true;
}