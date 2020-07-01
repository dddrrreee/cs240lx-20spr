// engler: trivial check that you can switch to user mode and back.
#include "single-step.h"

static int user_fn(void) {
    trace("user mode = %b, current mode=%b\n", USER_MODE, mode_get(cpsr_get()));
    assert(mode_is_user());
    for(int i = 0; i < 10; i++) 
        trace("hello %d\n", i);
    return 4;
}

void notmain() {
    cp14_enable();

    assert(mode_is_super());
    trace("about to try running at user level\n");
    int x = user_mode_run_fn(user_fn, STACK_ADDR2);
    assert(x == 4);
    trace("success!\n");
}
