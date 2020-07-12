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

static int fault_cnt = 1;

static void test_resume(int *addr, uint32_t val) {
    trace("1: expect 'domain section fault' for addr %p, at pc %p\n", addr, put32);
    // should get a fault here.
    put32(addr, val);
    fault_expected((uint32_t)put32, (uint32_t)addr, DOMAIN_SECTION_FAULT, fault_cnt++);

    trace("checking that %p == %d\n", addr, val);

    trace("2: expect 'domain section fault' for addr %p, at pc %p\n", addr, get32);
    assert(get32(addr) == val);
    fault_expected((uint32_t)get32, (uint32_t)addr, DOMAIN_SECTION_FAULT, fault_cnt++);

    trace("correct: %p == %d\n", addr, val);
}

int notmain_client() {
    printk("in notmain_client: going to kmalloc!\n");
    int *x = kmalloc(4);

    fault_cnt = last_fault_get().fault_count+1;

    printk("going to run test_resume\n");
    for(int i = 0; i < 10; i++)
        test_resume(x, i);
    return 0x12345678;
}

void notmain() {
    assert(!mmu_is_enabled());

    // we use memtrace_fn b/c you haven't done shadow memory etc.
    int x = memtrace_fn(notmain_client);
    assert(x == 0x12345678);

    assert(!mmu_is_enabled());

    trace_clean_exit("success!!\n");
}
