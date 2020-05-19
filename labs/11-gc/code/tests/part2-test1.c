// trivial program to start checking mark/sweep with.
//  1. allocate one block.
//  2. free it.
//  3. make sure no leaks detected.
#include "rpi.h"
#include "ckalloc.h"

void test1(void) {
    char *p = ckalloc(4);
    trace("alloc returned %p\n", p);
    memset(p, 0, 4);
    ckfree(p);
}

void notmain(void) {
    printk("GC test1\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    kmalloc_init_set_start(0x100000);
    unsigned n = 1024*1024;
    ck_init(kmalloc(n),n);

    // we call a function to do this so that the stack gets cleaned
    // up.
    test1();

    if(ck_heap_errors())
        panic("invalid error!!\n");
    else
        trace("SUCCESS heap checked out\n");

    if(ck_find_leaks(1))
        panic("should have no leaks!\n");
    else
        trace("SUCCESS: no leaks!\n");
}
