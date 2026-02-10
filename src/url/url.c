#include"url.h"

void init_url(url_t* url) {
    memset(url, 0, sizeof(*url));
}

int seperate_query(char* raw_path, url_t* url) {
    init_url(url);
    char dec_path[strlen(raw_path) + 1];
    if (decode_url(raw_path, dec_path) != 0) return 1;
    bool found = false, qeury_name = true;
    int j = 0;
    for (int i = 0; i < (int)strlen(dec_path); i++) {
        // query areas
        if (found) {

            // query seperator
            if (dec_path[i] == '&') {
                url->queries[url->query_count].value[j] = '\0';
                url->query_count++;
                j = 0;
                qeury_name = true;
                continue;
            }

            // key- value seperator
            if (dec_path[i] == '=') {
                url->queries[url->query_count].key[j] = '\0';
                j = 0;
                qeury_name = false;
                continue;
            }

            if (qeury_name) {
                url->queries[url->query_count].key[j++] = dec_path[i];
            } else {
                url->queries[url->query_count].value[j++] = dec_path[i];
            }

            continue;
        }

        // path ends here, queries are started from the next word..
        if (dec_path[i] == '?') {
            found = true;
            continue;
        }
        url->path[url->path_len++] = dec_path[i];
    }
    url->path[url->path_len] = '\0';
    if (found) {
        url->queries[url->query_count].value[j] = '\0';
        url->query_count++;
    }

    // printf("[SEP] query len=%d\n", url->query_count);

    return 0;
}