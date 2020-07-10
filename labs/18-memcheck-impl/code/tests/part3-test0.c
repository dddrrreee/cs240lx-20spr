// engler: check that allocation works with shadow memory: allocate, do a bunch
// of writes and read the result.
#include "memcheck.h"
#include "cpsr-util.h"

static void test_resume(int *addr, uint32_t val) {
    memcheck_alloc(4);

    trace("expect 'domain section fault' for addr %p, at pc %p\n", addr, put32);
    // should get a fault here.
    put32(addr, val);
    trace("checking that %p == %d\n", addr, val);

    assert(get32(addr) == val);
    trace("correct: %p == %d\n", addr, val);
}

unsigned expected = 45;

int notmain_client() {
    volatile int *x = memcheck_alloc(4);

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
