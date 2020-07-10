// engler: check that allocation works with shadow memory: allocate, do a bunch
// of writes and read the result.
#include "memcheck.h"
#include "cpsr-util.h"

unsigned expected = 45;

int notmain_client() {
    volatile int *x = memcheck_alloc(4);
    *x = 0;

    for(int i = 0; i < 10; i++)
        *x += i;
    printk("*x = %d\n", *x);
    assert(get32(x) == expected);
    return *x;
}

void notmain() {
    assert(!mmu_is_enabled());

    int x = memcheck_fn(notmain_client);
    assert(!mmu_is_enabled());

    assert(x == expected);


    trace_clean_exit("success!!\n");
}
