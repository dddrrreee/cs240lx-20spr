// engler: make sure you detect use after free
#include "memcheck.h"

int notmain_client() {
    int *x = memcheck_alloc(4);
    memcheck_free(x); 
    *x = 1; // should get an error here.
    return 0;
}

void notmain() {
    assert(!mmu_is_enabled());
    memcheck_fn(notmain_client);
    panic("should not get here: free error\n");
}
