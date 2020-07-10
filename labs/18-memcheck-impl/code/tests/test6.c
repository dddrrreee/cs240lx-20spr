// engler: keep taking traps using single-stepping: you will need to:
//  1. catch the domain fault;
//  2. change the permissions to allow r/w for track_id
//  3. set up single step mode.
//  4. jump back.
//  5. in the single step handler, restore permissions.
//
// XXX: need to add checks that we caught the right exception.
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
