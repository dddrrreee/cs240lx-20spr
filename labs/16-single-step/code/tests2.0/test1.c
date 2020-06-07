// this is the same as 16-part1-test3.c except that we use user_trampoline_ret.
#include "rpi.h"
#include "rpi-interrupts.h"
#include "rpi-constants.h"
#include "cp14-debug.h"

static volatile int done = 0, cnt;

static void *expected_handle;

// see how big you can make this!
#define N 10000
void user_test(void *handle)  {
    assert(handle == expected_handle);
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
        output("done single stepping: cnt=%d!!\n", cnt);
        brkpt_mismatch_disable0(pc);
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

    brk_verbose(1);

    uint32_t cpsr_old = cpsr_get();
    output("current mode = %s (%x)\n", mode_str(cpsr_old), cpsr_old);

    // 0 should always mismatch
    brk_debug("about to set mismatch brkpt\n");
    brkpt_mismatch_set0(0, single_step_handler);
    brk_debug("set mismatch brkpt\n");

    // if you want more output, change this.
    brk_verbose(0);
    expected_handle = &expected_handle;
    user_trampoline_ret(mode_set(cpsr_old, USER_MODE), user_test, expected_handle);

    // make sure that the cpsr is set correctly.
    output("expected mode = %s (%x)\n", mode_str(cpsr_old), cpsr_old);
    cpsr_old = cpsr_get();
    output("current mode = %s (%x)\n", mode_str(cpsr_get()), cpsr_old);

    assert(cpsr_old == cpsr_get());
    assert(cnt == N);

    // should probably do more checks that the registers are ok.
    exit_success("trampoline returned successfully\n");
}
