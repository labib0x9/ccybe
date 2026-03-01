#ifndef MIME_H
#define MIME_H

#include<stdio.h>
#include<string.h>
#include<strings.h>

enum {
    JS,
    HTML,
    CSS,
    PNG,
};

int get_mime(char *path);

#endif