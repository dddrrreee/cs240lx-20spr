// engler: check that allocation works with shadow memory: allocate, do a bunch
// of writes and read the result.
#include "memcheck.h"
#include "cpsr-util.h"

unsigned expected = 45;

int notmain_client() {
    // zero initialized by kmalloc.
    volatile int *x = memcheck_alloc(4);
    for(int i = 0; i < 10; i++)
        *x += i;
    printk("*x = %d\n", *x);

    // we don't check any more thoroughly.
    trace("expect 'domain section fault' for addr %p, at pc %p\n", x, get32);
    assert(get32(x) == expected);
    fault_expected((uint32_t)get32, (uint32_t)x, DOMAIN_SECTION_FAULT, 0);
    return *x;
}

void notmain() {
    assert(!mmu_is_enabled());

    int x = memcheck_fn(notmain_client);
    assert(!mmu_is_enabled());

    assert(x == expected);


    trace_clean_exit("success!!\n");
}
