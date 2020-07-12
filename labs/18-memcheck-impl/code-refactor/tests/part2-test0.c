// engler: keep taking traps using single-stepping
#include "memcheck.h"
#include "cpsr-util.h"

int notmain_client() {
    assert(mode_is_user());

    // note: if you try to run this with memcheck, will fail b/c this memory is
    // not allocated.
    int v, *x = (void*)0x200000;

    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, put32);
    put32(x, 5);
    fault_expected((uint32_t)put32, (uint32_t)x, DOMAIN_SECTION_FAULT, 1);

    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, put32);
    put32(x, 5);
    fault_expected((uint32_t)put32, (uint32_t)x, DOMAIN_SECTION_FAULT, 2);

    assert(get32(x) == 5);
    fault_expected((uint32_t)get32, (uint32_t)x, DOMAIN_SECTION_FAULT, 3);
    return 0x12345678;
}

void notmain() {
    assert(!mmu_is_enabled());
    int x = memtrace_fn(notmain_client);
    assert(x == 0x12345678);
    assert(!mmu_is_enabled());

    trace_clean_exit("success!!\n");
}
