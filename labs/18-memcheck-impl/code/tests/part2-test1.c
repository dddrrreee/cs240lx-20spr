// engler: make sure you can restart checking multiple times.
#include "memcheck.h"
#include "cpsr-util.h"

int notmain_client() {
    assert(mode_is_user());
    int v, *x = (void*)0x200000;
    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, put32);
    put32(x, 5);
    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, put32);
    put32(x, 5);

    assert(get32(x) == 5);
    return 0x12345678;
}

static void run(void) {
    assert(!mmu_is_enabled());
    int x = memcheck_fn(notmain_client);
    assert(x == 0x12345678);
    assert(!mmu_is_enabled());
}

void notmain() {
    // keep running memcheck_fn --- make sure you don't multiple init.
    for(int i = 0; i < 10; i++) 
        run();
    trace_clean_exit("success!!\n");
}
