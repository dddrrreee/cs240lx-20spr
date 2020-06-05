#ifndef __INTERLEAVE_CHECK_H__
#define __INTERLEAVE_CHECK_H__
// engler: simple concurrency interleaving checking for cs240lx

#include "rpi.h"
#include "rpi-interrupts.h"
#include "rpi-constants.h"
#include "cp14-debug.h"

typedef struct checker { 
    /************************************************************
     * these fields are set by the user
     */
    // if you need state.
    volatile void *state;

    // can use nested functions if you want to pass data.
    void (*A)(struct checker *);
    void (*B)(struct checker *);

    // initialize the state.
    void (*init)(struct checker *c);
    // check that the state is valid.
    int  (*check)(struct checker *c);

    /************************************************************
     * these fields are for internal use by the checker.
     */

    // total number of switches we have done for all checking.
    volatile unsigned nswitches;

    /* the fields below are used by each individual run */

    // context switch from A->B on instruction <n>.
    // starts at 1 (run 1 instruction and switch)
    // then      2 (run 2 instructions and switch)
    // then      3 (run 3 instructions and switch)
    // ...
    // finished when we run and do not switch.
    //
    // lots of speed opportunities here.
    // 
    // Extension: handle more than one switch.
    // Extension: only switch when A does a store.
    //            all other instructions should not
    //            have an impact (I *believe* need to 
    //            check through the ARM carefully).
    volatile unsigned switch_on_inst_n; 

    // total number of instructions (faults) we ran so far on a given
    // run: used by the interrupt handler to see if we should 
    // switch to B() (when inst_count == switch_on_inst_n)
    volatile uint32_t inst_count;

    // the address we switched on this time: used for error reporting
    // when you get a mismatch.
    volatile uint32_t switch_addr;

    // set by the breakpoint handler when a switch occurs.
    // reset on each checking run: if you run and does not get 
    // set, you are done.
    volatile unsigned switched_p;    

    // records the total number of trials and errors.
    unsigned ntrials;
    unsigned nerrors;

    // set when we start doing interleave checking.
    unsigned interleaving_p;
} checker_t;

// check the routines A and B pointed to in <c>
// returns 1 if was successful, 0 otherwise.
//  total trials and errors can be pulled from <c>
int check(checker_t *c);

#endif
