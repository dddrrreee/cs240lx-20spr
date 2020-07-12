// engler: take one domain trap and resume (sanity check).
#include "memcheck.h"

int notmain_client() {
    assert(mode_is_user());
    int v, *x = (void*)0x200000;
    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, put32);
    put32(x, 5);
    fault_expected((uint32_t)put32, (uint32_t)x, DOMAIN_SECTION_FAULT, 1);

    // note: if you are developing this as laid out in the lab, you won't get this
    // fault.  could add this stuff back in.
    // trace("expect 'domain section fault' for addr %p, at pc %p\n", x, get32);
    assert(get32(x) == 5);
    // fault_expected((uint32_t)get32, (uint32_t)x, DOMAIN_SECTION_FAULT, 2);
    return 0x12345678;
}

void notmain() {
    assert(!mmu_is_enabled());

    int x = memtrace_fn(notmain_client);
    assert(x == 0x12345678);

    assert(!mmu_is_enabled());

    trace_clean_exit("success!!\n");
}
