// engler: make sure we can trap/resume multiple times.
#include "memcheck.h"

static void test_resume(uint32_t addr, uint32_t val) {
    // currently we just turn on / off entire trapping system.
    memcheck_trap_enable();

    trace("expect 'domain section fault' for addr %p, at pc %p\n", addr, PUT32);
    // should get a fault here.
    PUT32(addr, val);

    trace("checking that trapping is disabled\n");
    assert(!memcheck_trap_enabled());
    trace("correct: disabled\n");

    fault_expected((uint32_t)PUT32, addr, DOMAIN_SECTION_FAULT);

    trace("checking that %p == %d\n", addr, val);

    trace("expect 'domain section fault' for addr %p, at pc %p\n", addr, GET32);
    memcheck_trap_enable();


    assert(GET32(addr) == val);
    trace("correct: %p == %d\n", addr, val);
    assert(!memcheck_trap_enabled());

    fault_expected((uint32_t)GET32, addr, DOMAIN_SECTION_FAULT);
}

void notmain() {
    memcheck_init();
    memcheck_on();
    assert(mmu_is_enabled());

    uint32_t sp = STACK_ADDR2;

    // allocates a mapping; mark as no access.
    memcheck_track(sp);

    // hack to do a simple unit test that we can continue after a fault.
    memcheck_continue_after_fault();

    for(int i = 0; i < 10; i++) 
        test_resume(sp, i);

    trace_clean_exit("success!!\n");
}
