#include "util.h"

#include <stdlib.h>
#include <string.h>

char* strdup(const char* s) {
    if (s == NULL) {
        return NULL;
    }
    const size_t len = strlen(s);
    char* ret = malloc(len + 1);
    memcpy(ret, s, len + 1);
    return ret;
}

void strappend(char** dst, const char* src) { strnappend(dst, src, strlen(src)); }

void strnappend(char** dst, const char* src, size_t n) {
    strncat(*dst, src, n);
    *dst += n;
}
