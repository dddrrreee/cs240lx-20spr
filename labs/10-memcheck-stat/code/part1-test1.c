// trivial program to start: allocate a single block, free it, check the
// heap
//
// i'd start by making sure you get the identical values for:
// ckalloc, then ckfree, then ck_heap_errors.
//
// you can do by running
//      "make testall"
// or adding testall to the makefile "all:" target.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test1\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    ck_init((void*)0x100000, 1024 * 1024);

    int *p = ckalloc(4);
    trace("alloc returned %p\n", p);
    memset(p, 0, 4);
    ckfree(p);

    if(ck_heap_errors())
        panic("invalid error!!\n");
    else
        trace("SUCCESS heap checked out\n");


    memset(p, 0, 5);

    if(ck_heap_errors() != 1)
        panic("invalid error!!\n");
    else
        trace("SUCCESS: detected corruption\n");

}
