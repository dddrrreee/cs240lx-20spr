// engler, make sure we can handle a fault: use your code from lab 14 in
// cs140e.
#include "memcheck.h"

void notmain() {
    memcheck_init();
    memcheck_on();
    assert(mmu_is_enabled());

    // STACK_ADDR2 has never been mapped: storing to it
    // should give a fault.
    uint32_t *sp = (void*)STACK_ADDR2;
    trace("expect datafault xlate fault at address: %p at pc %p\n", 
        sp, put32);

    // fault pc should be in put32
    put32(sp, 1);

    mem_panic("should not get here!!\n");
}
