#include"mime.h"

int get_mime(char *path) {
    char ext[10];
    int n = strlen(path), ext_len = 0;
    for (int i = n - 1; i >= 0; i--) {
        if (path[i] == '.') { break; }
        ext[ext_len++] = path[i];
    }

    for (int i = 0; i < ext_len / 2; i++) {
        char x = ext[i];
        ext[i] = ext[ext_len - i - 1];
        ext[ext_len - i - 1] = x;
    }

    ext[ext_len] = '\0';

    printf("[EXT] = %s\n", ext);

    // #ifdef __APPLE__
        if (strcasecmp(ext, "js") == 0) return JS;
        if (strcasecmp(ext, "css") == 0) return CSS;
        if (strcasecmp(ext, "html") == 0) return HTML;
        if (strcasecmp(ext, "png") == 0) return PNG;
        return -1;
    // #elif defined(__linux__)
    //     if ()
    // #endif
}