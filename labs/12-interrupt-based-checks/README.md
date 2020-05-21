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

***STILL NEED TO WRITE BELOW***

In this part, integrate the timer code with your checking code.
  1. You should check if the interrupted pc was within your memory 
     checking code.  You will need to add an empty routine `gc_code_begin`
     at the beginning and `gc_code_end` at the end, similarly for your
     memory allocator.
  2. If the interrupt pc is within these bounds, skip checking and increment a `skipped` counter.
  3. If not, run the checking and increment a `checking` counter.

Implement three routines:
  1. `stat_check_init`: do any initialization.
  2. `stat_check_on`: allow it to start checking.
  3. `stat_check_off`: turn off checking.

They should be able to turn on checking and turn it off repeatedly.  They should
also `panic` if `stat_check_init` was not called.

I'm not sure the best way to say how frequent --- have them compute the clock speed?
etc?

Have them put their leak checking in source?
Add to cstart.
Run some old programs (not sure which ones).

You should be able to run the checks in part 1 and have them pass.
These merely check that given output of any program, adding the checking does not change
them.
    You can slip this into any 
Hmm.  Maybe we should have them modify?  Add to cstart.  Then automatically handle.

Maybe have them rerun over and over?

----------------------------------------------------------------------
## Part 2: interrupt based checking.
