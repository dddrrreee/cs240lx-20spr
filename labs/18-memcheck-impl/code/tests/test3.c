// engler: keep taking traps using single-stepping
#include "memcheck.h"
#include "cpsr-util.h"

int notmain_client() {
#if 0
    printk("in notmain_client: going to kmalloc!\n");
    int *x = kmalloc(4);
#endif

    assert(mode_is_user());
    int v, *x = (void*)0x200000;
    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, put32);
    put32(x, 5);
    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, put32);
    put32(x, 5);

    assert(get32(x) == 5);
    return 0x12345678;
}

void notmain() {
    assert(!mmu_is_enabled());
    int x = memcheck_fn(notmain_client);
    assert(x == 0x12345678);
    assert(!mmu_is_enabled());

    trace_clean_exit("success!!\n");
}
