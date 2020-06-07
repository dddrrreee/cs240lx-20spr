// same as test6, just a bit more stringent checking.
#include "rpi.h"
#include "rpi-interrupts.h"
#include "rpi-constants.h"
#include "cp14-debug.h"

static volatile int done = 0, start = 0;

// single step through this code: 
static void user_test(void)  {
    start = 1;

    asm volatile ("nop"); // 1
    asm volatile ("nop"); // 2
    asm volatile ("nop"); // 3
    asm volatile ("nop"); // 4
    asm volatile ("nop"); // 5

    asm volatile ("nop"); // 1
    asm volatile ("nop"); // 2
    asm volatile ("nop"); // 3
    asm volatile ("nop"); // 4
    asm volatile ("nop"); // 5

    asm volatile ("nop"); // 1
    asm volatile ("nop"); // 2
    asm volatile ("nop"); // 3
    asm volatile ("nop"); // 4
    asm volatile ("nop"); // 5

    asm volatile ("nop"); // 1
    asm volatile ("nop"); // 2
    asm volatile ("nop"); // 3
    asm volatile ("nop"); // 4
    asm volatile ("nop"); // 5



    // done single stepping: tell it to die.
    done = 1;

    panic("should not get here!!\n");
}

// breakpoint exception.
//
// has more code b/c we are checking that you match each pc at a time.
// (in hindsight would have been easier to just cross-check)
//
static void single_step_handler(uint32_t regs[16], uint32_t pc, uint32_t addr) {
    static unsigned matches = 0;
    static volatile uint32_t addr_expected = 0;

    brk_debug("prefetch abort at %p, user_test = %p\n", pc, user_test);
    if(!was_debug_instfault())
        panic("impossible: should get no other faults\n");

    if(start) {
        // if we haven't started checking yet, set the expected addr.
        if(!addr_expected) {
            assert(pc >= (uint32_t)user_test && pc < (uint32_t)single_step_handler);
            addr_expected = pc;
            brk_debug("START: setting to %p\n", addr_expected);
        } 
        brk_debug("expected = %p, pc = %p\n", addr_expected, pc);
        assert(addr_expected == pc);
        addr_expected += 4;
        matches++;
    }

    // if done exit.
    if(done) {
        // need to match at least 20 nops, plus a bunch of other stuff.
        assert(matches > 20);
        exit_success("done single stepping, got %d matches!\n", matches);
    // else set a mis-match for the next instruction.
    } else {
        brk_debug("setting up a mismatch on %p\n", pc);
        brkpt_mismatch_set0(pc, single_step_handler);
        brk_debug("done setting up a mismatch on %p\n", pc);
    }
}

void notmain(void) {
    cp14_enable();

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
