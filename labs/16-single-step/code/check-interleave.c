// engler: very simple concurrency checker for cs240lx
#include "check-interleave.h"

// used to communicate with the breakpoint handler.
static checker_t *checker = 0;

// breakpoint.
static void single_step_handler(uint32_t regs[16], uint32_t pc, uint32_t addr) {
    brk_debug("prefetch abort at %p\n", pc);
    if(!was_debug_instfault())
        panic("impossible: should get no other faults\n");

    checker_t *c = checker;

    // not exactly right: we consider each fault 1 inst.
    if(c->inst_count == c->switch_on_inst_n) {
        output("about to switch from %p to %p\n", pc, c->B);

        // 1. disable mismatch.
        // 2. set <switch_addr>, <switched_p>, <nswitches>
        // 3. call B.
        unimplemented();

        output("done running B, going to return to A\n");
    } else {
        assert(c->inst_count < c->switch_on_inst_n);
        c->inst_count++;

        brk_debug("setting up a mismatch on %p\n", pc);
        unimplemented();
        brk_debug("done setting up a mismatch on %p\n", pc);
    }
}

static unsigned check_one_cswitch(checker_t *c) {
    uint32_t cpsr_old = cpsr_get();
    uint32_t cpsr_user = mode_set(cpsr_old, USER_MODE);

    checker = c;
    c->interleaving_p = 1;
    do {
        c->init(c);

        brk_debug("trial=%d: about to set mismatch brkpt for A\n", c->ntrials);
        // set a mismatch on 0:  should always mismatch
        unimplemented();

        // switch on the next instruction: note, b/c of non-determ,
        // this does not mean we do the same prefix as previous trials.
        c->switch_on_inst_n++;
        c->inst_count = 0;
        c->switched_p = 0;

        // use <user_trampoline_ret> to start running c->A
        unimplemented();

        if(!c->check(c)) {
            output("ERROR: check failed when switched on address %p, after %d instructions\n",
                c->switch_addr, c->switch_on_inst_n);
            c->nerrors++;
        }

        c->ntrials ++;

    } while(c->switched_p);

    printk("done!  did c->nswitches=%d, ntrials=%d, nerrors=%d\n", 
                    c->nswitches, c->ntrials, c->nerrors);

    // make sure that the cpsr is set correctly.
    output("expected mode = %s (%x)\n", mode_str(cpsr_old), cpsr_old);
    output("current mode = %s (%x)\n", mode_str(cpsr_get()), cpsr_get());
    // should be redundant.
    brkpt_mismatch_disable0(0);

    assert(mode_get(cpsr_old) == mode_get(cpsr_get()));

    // tell code we are not doing interleave checking anymore.

    c->interleaving_p = 0;
    // when we get back here, it might have breakpoints enabled: disable them.
    return c->nerrors == 0;
}

// should not fail: testing sequentially.
// if the code is non-deterministic, or the init / check
// routines are busted, this could find it.
static unsigned check_sequential(checker_t *c) {
    // we don't count sequential trials.
    unsigned i;
    for(i = 0; i < 100; i++) {
        c->init(c);

        c->A(c);
        c->B(c);
        if(!c->check(c))
            panic("check failed sequentially: code is broken\n");

#if 0
        // if AB commuted, we could do this: won't be true in general.
        c->init(c);
        c->B(c);
        c->A(c);
        if(!c->check(c))
            panic("check failed sequentially: code is broken\n");
#endif
    }
    return 1;
}

// zero out all the fields that might be left over from previous run.
static void check_zero(checker_t *c) {
    c->nswitches =
    c->switch_on_inst_n =
    c-> inst_count =
    c->switch_addr = 
    c->switched_p =
    c->ntrials =
    c->nerrors =
    c->interleaving_p = 0;
}

int check(checker_t *c) {
    cp14_enable(); // dunno if we should keep doing this on multiple runs.
    check_zero(c);
    return check_sequential(c) && check_one_cswitch(c);
}
