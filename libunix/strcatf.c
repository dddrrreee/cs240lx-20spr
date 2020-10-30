#include <stdarg.h>
#include <string.h>
#include "libunix.h"

// XXX: fix this and combine.

char *strcatf(char *dst, const char *fmt, ...) {
    char buf[4096];

    va_list args;
    va_start(args, fmt);
        if(vsnprintf(buf, sizeof buf, fmt, args) < 0)
            panic("overflowed buffer\n");
    va_end(args);
    return strcat(dst, buf);
}

char *strcpyf(char *dst, const char *fmt, ...) {
    char buf[4096];

    va_list args;
    va_start(args, fmt);
        if(vsnprintf(buf, sizeof buf, fmt, args) < 0)
            panic("overflowed buffer\n");
    va_end(args);
    return strcpy(dst, buf);
}

char *strdupf(const char *fmt, ...) {
    char buf[4096];

    va_list args;
    va_start(args, fmt);
        if(vsnprintf(buf, sizeof buf, fmt, args) < 0)
            panic("overflowed buffer\n");
    va_end(args);
    return strdup(buf);
}
