// engler: call a null routine.
#include "memcheck.h"

int notmain_client() {
    return 0x12345678;
}

void notmain() {
    assert(!mmu_is_enabled());
    int x = memcheck_fn(notmain_client);
    assert(x == 0x12345678);
    assert(!mmu_is_enabled());
    trace_clean_exit("success!!\n");
}
