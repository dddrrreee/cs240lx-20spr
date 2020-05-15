// use after free.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test1\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    ck_init((void*)0x100000, 1024 * 1024);

    char *p = ckalloc(4);
    memset(p, 0, 4);
    ckfree(p);  // should catch this.

    p[0] = 1;   // past end of block
    if(ck_heap_errors() != 1)
        panic("invalid error!!\n");
    else
        trace("SUCCESS: detected corruption\n");
}
