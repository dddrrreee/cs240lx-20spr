// engler: do a single trap and make sure we can resume.
#include "memcheck.h"

void notmain() {
    memcheck_init();
    memcheck_on();
    assert(mmu_is_enabled());

    // <STACK_ADDR2> should be mapped by memcheck_init()
    uint32_t sp = STACK_ADDR2;

    // allocates a mapping; mark as no access.
    memcheck_track(sp);


    // hack to do a simple unit test that we can continue after a fault.
    memcheck_continue_after_fault();

    // currently we just turn on / off entire trapping system.
    memcheck_trap_enable();

    trace("expect 'domain section fault' for addr %p, at pc %p\n", sp, PUT32);

    // should get a fault here.
    unsigned val = 4;
    PUT32(sp, val);

    trace("resumed after fault: checking that trapping is disabled\n");
    assert(!memcheck_trap_enabled());
    trace("correct: disabled\n");
    fault_expected((uint32_t)PUT32, sp, DOMAIN_SECTION_FAULT);

    // don't enable checking.
    trace("checking that %p == %d\n", sp, val);
    assert(GET32(sp) == val);
    trace("correct: %p == %d\n", sp, val);

    trace_clean_exit("success!!\n");
}
