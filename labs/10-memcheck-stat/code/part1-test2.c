// checks a single memory overflow.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test1\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    ck_init((void*)0x100000, 1024 * 1024);

    char *p = ckalloc(4);
    memset(p, 0, 4);
    p[4] = 1;   // past end of block
    ckfree(p);  // should catch this.
}
