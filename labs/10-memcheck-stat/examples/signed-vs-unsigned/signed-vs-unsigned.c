#include "rpi.h"

enum { X = 0xfe };

void notmain(void) {
    char c = X;
    int y =  X;
    assert(c == y);

    unsigned char uc = X;
    assert(uc == y);

    signed char sc = X;
    assert(sc == y);
}
