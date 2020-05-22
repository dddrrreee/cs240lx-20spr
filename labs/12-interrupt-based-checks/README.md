## Doing checks using interrupts (and binary rewriting)

Today we're going to build a simple interrupt-based checker for finding
heap corruption.  We'll extend it next time so it gives you closer to
Purify-functionality, but today we build toy examples of most of the
pieces we'll need and then build out key pieces.

Today:
  1. Look over the interrupt code (`timer-int`) --- it's from last cs140e, so 
     should be familiar to most of you.  Note that some of the code it uses
     is in `libpi/staff-src` and `libpi/include` --- check there if you can't
     find it.  And, if you're not already, start using tags!
 
  2. You'll adapt this code to do simple heap corruption checking by calling into
     your heap checker (and leak checker if that is working) in the interrupt
     handler.  We'll maintain a range of code addresses we check (default: the 
     entire program) and a range of code addresses we do not check (initially:
     the entire `ckalloc.c` file) --- if you get interrupted in the former,
     run your heap checker and see if you can detect errors.

  3. As you notice, interrupt checking lets you transparently check an
     arbitrary predicate with *guaranteed* regularlity without any
     code modifications.  This hack can serve you well in real life.
     However, a downside is that we will rarely get lucky and interrupt
     exactly when the invariant breaks (we can also miss if it breaks
     briefly).  We'll do a conceptually simple hack: for each load or
     store we discover (the two operations that can violate invariants)
     we replace them with a branch instruction that jumps to custom
     checking routine for that specific instruction address.  Because the
     jump is the same size as the load or store, the program binary does
     not change size ("dialate") and so we do not have to patch program
     addresses (infeasible if you can't reliably classify code and data).
     Because the trampoline is customized to that specific instruction,
     we can hardcode any knowledge it needs, including the return address
     it needs to jump back to.

     This means that when we interrupt a load or store once, the check
     will be performed for all future executions.  I.e., it is sticky,
     or persistent.

  4. While step (3) gives us much more contol over the program, there
     are large blind spots until we instrument everything --- we will
     miss large chunks of code that only run for a short while or do
     not repeat.  We will build a trivial "restore-and-replay" setup to
     your code that will allow you restart your progam over and over,
     until you have covered all of its code.  You can then check it on
     a thorough, final run.

     Checkpoint-and-restore is a powerful technique that has many
     applications but is also (IMO) wildly underutilized because it
     is extremely challenging to build on today's complex systems.
     This is another case of: simple pi system = simple implementation.

----------------------------------------------------------------------
## Part 1: interrupt based checking.

In this part, integrate the timer code with your checking code.
  1. You should check if the interrupted pc was within your memory 
     checking code.  You can figure out the code range by putting
     a sentinal routine at the start of `ckalloc.c`:

            void ckalloc_start(void) {}

     and a sentinal routine at the end:

            void ckalloc_end(void) {}

     And considering all code between them as not-checkable. 
     Note that we use a special `gcc` option so it does not
    reorder the routines!

  2. If the interrupt pc is within these bounds, skip checking and increment a `skipped` counter.
  3. If not, run the checking and increment a `checking` counter.

Implement the routines in `ck-memcheck.c` (described at the end of `ckalloc.h`).
Key routines:
  1. `ck_mem_init`: do any initialization.
  2. `ck_mem_on`: start checking.
  3. `ck_mem_off`: turn off checking.

Clients should be able to turn on checking and turn it off repeatedly.
They should also `panic` if `stat_check_init` was not called.

You'll have to copy in and modify the timer code.  Play around with the test to see
how quickly you can catch the error.  Also perhaps add other tests and other errors.

Notes:
    1. You probably should make a way to easily shrink down the clock period in the timer
       intrrupt.
    2. As you make interrupts more frequent, your code can appear to "lock up" b/c it
       is either running too slowly or b/c you are doing something you should not in 
       the interrupt handler.
    3. If your interrupt handler uses data that is shared with non-interrupt code
       make sure it is `volatile` or you just do `put32` and `get32`.


Extension:
  1. Make it so you can transparently run all your old tests and have them pass.  A nice 
     thing about our checker is that it should not alter the behavior of existing programs
     other than making them slower.  So it's easy to do lots and lots of regressions.
     (Perhaps the easiest way to do this is modifying `cstart`; even slicker is to modify
      the `.bin`.)

----------------------------------------------------------------------
## Part 2: binary rewriting
