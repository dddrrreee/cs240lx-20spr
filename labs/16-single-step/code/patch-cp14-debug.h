

/********************************************************************
 ********************************************************************
 * lab-16: single-stepping.
 */


/********************************************************************
 * 
 * part 1: trivial single step.
 *  16-part1-test1. 16-part1-test2.c 16-part1-test3.c
 */

// simple debug macro: can turn it off/on by calling <brk_verbose({0,1})>
#define brk_debug(args...) if(brk_verbose_p) debug(args)

extern int brk_verbose_p;
static inline void brk_verbose(int on_p) { brk_verbose_p = on_p; }

// Set:
//  - cpsr to <cpsr> 
//  - sp to <stack_addr>
// and then call <fp>.
//
// Does not return.
//
// used to get around the problem that if we switch to USER_MODE in C code,
// it will not have a stack pointer setup, which will cause all sorts of havoc.
void user_trampoline_no_ret(uint32_t cpsr, void (*fp)(void));

// set a mismatch on <addr> --- call <handler> on mismatch.
// NOTE:
//  - an easy way to mismatch the next instruction is to call with
//    use <addr>=0.
//  - you cannot get mismatches in "privileged" modes (all modes other than
//    USER_MODE)
//  - once you are in USER_MODE you cannot switch modes on your own since the 
//    the required "msr" instruction will be ignored.  if you do want to 
//    return from user space you'd have to do a system call ("swi") that switches.
void brkpt_mismatch_set0(uint32_t addr, handler_t handler);

/********************************************************************
 * 
 * part 2: concurrency checking.
 *  16-part2-test1. 16-part2-test2.c 16-part2-test3.c
 */


// disable mismatch breakpoint <addr>: error if doesn't exist.
void brkpt_mismatch_disable0(uint32_t addr);

// get the saved status register.
static inline uint32_t spsr_get(void) {
    unimplemented();
}
// set the saved status register.
static inline void spsr_set(uint32_t spsr) {
    unimplemented();
}


// same as user_trampoline_no_ret except:
//  -  it returns to the caller with the cpsr set correctly.
//  - it calls <fp> with a user-supplied pointer.
//  16-part2-test1.c
void user_trampoline_ret(uint32_t cpsr, void (*fp)(void *handle), void *handle);
