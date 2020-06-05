// should not fail: just checking that loops and stuff work.
#include "rpi.h"
#include "rpi-interrupts.h"
#include "rpi-constants.h"
#include "cp14-debug.h"

static volatile int done = 0, cnt;

// see how big you can make this!
#define N 10000
void user_test(void)  {
    for(int i = 0; i < N; i++) 
        cnt++;
    printk("testing printk!\n");
    done = 1;
}

// breakpoint.
static void single_step_handler(uint32_t regs[16], uint32_t pc, uint32_t addr) {
    brk_debug("prefetch abort at %p, user_test = %p\n", pc, user_test);

    if(!was_debug_instfault())
        panic("impossible: should get no other faults\n");

    if(done)  {
        assert(cnt == N);
        exit_success("done single stepping: cnt=%d!!\n", cnt);
    } else {
        brk_debug("setting up a mismatch on %p\n", pc);
        brkpt_mismatch_set0(pc, single_step_handler);
        brk_debug("done setting up a mismatch on %p\n", pc);
    }
}

void notmain(void) {
    cp14_enable();

    // try with enable cache.
    enable_cache();

    uint32_t cpsr_old = cpsr_get();

    brk_verbose(1);
    brk_debug("current mode = %s\n", mode_str(cpsr_get()));

    // 0 should always mismatch
    brk_debug("about to set mismatch brkpt\n");
    brkpt_mismatch_set0(0, single_step_handler);
    brk_debug("set mismatch brkpt\n");

    // if you want more output, change this.
    brk_verbose(0);
    user_trampoline_no_ret(mode_set(cpsr_old, USER_MODE), user_test);

    panic("FAILED: should not get here\n");
}
