// engler: trivial check that you can switch to user mode and back.
#include "single-step.h"

static int user_fn(void) {
    asm volatile("nop");  // 1
    asm volatile("nop");  // 2
    asm volatile("nop");  // 3
    asm volatile("nop");  // 4
    asm volatile("nop");  // 5

    asm volatile("nop");  // 1
    asm volatile("nop");  // 2
    asm volatile("nop");  // 3
    asm volatile("nop");  // 4
    asm volatile("nop");  // 5

    return 0xdeadbeef;
}

void notmain() {
    single_step_init();

    assert(mode_is_super());
    trace("about to try running at user level\n");
    unsigned x = single_step_run(user_fn, STACK_ADDR2);
    assert(x == 0xdeadbeef);
    assert(mode_is_super());
    trace("success: %d single steps (i got 17)!\n", single_step_cnt());
}
