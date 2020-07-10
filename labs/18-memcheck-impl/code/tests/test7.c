// engler: make sure you detect use after free
//
// XXX: need to add checks that we caught the right exception.
#include "memcheck.h"
#include "cpsr-util.h"

int notmain_client() {
    int *x = memcheck_alloc(4);
    memcheck_free(x);
    *x = 1;
    return 0;
}

void notmain() {
    assert(!mmu_is_enabled());
    memcheck_fn(notmain_client);
    panic("should not get here: free error\n");
}
