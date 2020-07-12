// engler: make sure you can restart checking multiple times.
#include "memcheck.h"
#include "cpsr-util.h"

static int fault_cnt = 1;

int notmain_client() {
    assert(mode_is_user());
    int v, *x = (void*)0x200000;
    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, put32);
    put32(x, 5);
    fault_expected((uint32_t)put32, (uint32_t)x, DOMAIN_SECTION_FAULT, fault_cnt++);


    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, put32);
    put32(x, 5);
    fault_expected((uint32_t)put32, (uint32_t)x, DOMAIN_SECTION_FAULT, fault_cnt++);

    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, get32);
    assert(get32(x) == 5);
    fault_expected((uint32_t)get32, (uint32_t)x, DOMAIN_SECTION_FAULT, fault_cnt++);
    return 0x12345678;
}

static void run(void) {
    assert(!mmu_is_enabled());
    int x = memtrace_fn(notmain_client);
    assert(x == 0x12345678);
    assert(!mmu_is_enabled());
}

void notmain() {
    // keep running memcheck_fn --- make sure you don't multiple init.
    for(int i = 0; i < 10; i++) 
        run();
    trace_clean_exit("success!!\n");
}
