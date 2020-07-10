// engler: make sure we can trap/resume multiple times.
#include "memcheck.h"

static void test_resume(int *addr, uint32_t val) {
    trace("expect 'domain section fault' for addr %p, at pc %p\n", addr, put32);
    // should get a fault here.
    put32(addr, val);
    trace("checking that %p == %d\n", addr, val);

    assert(get32(addr) == val);
    trace("correct: %p == %d\n", addr, val);
}

int notmain_client() {
    printk("in notmain_client: going to kmalloc!\n");
    int *x = kmalloc(4);
    printk("going to run test_resume\n");
    for(int i = 0; i < 10; i++) 
        test_resume(x, i);
    return 0x12345678;
}

void notmain() {
    assert(!mmu_is_enabled());

    int x = memcheck_fn(notmain_client);
    assert(x == 0x12345678);

    assert(!mmu_is_enabled());

    trace_clean_exit("success!!\n");
}
