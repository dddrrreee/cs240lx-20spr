## Lab: single-stepping to make simple, powerful tools


-----------------------------------------------------------------------------

***NOTE: the original lab was buggy!!!***

***NOTE: the original lab was buggy!!!***

***NOTE: the original lab was buggy!!!***

***NOTE: the original lab was buggy!!!***

***NOTE: the original lab was buggy!!!***

***NOTE: the original lab was buggy!!!***

Many people were having weird state problems.  I think this was mainly because
I tried to cheap-out and just use the normal stack for running "A()"
in `USER` mode.   This, of course, was dumb:   
   - When you start calling system calls, you will switch back to `SUPER` mode.
     It will start using it's private SUPER stack pointer.  
   - Of course, "A()" has also been using the same stack, and been updating 
     its USER stack pointer.
   - Unfortunately, ***the system call uses the SUPER sp and so will not see
     the USER updated sp***   
   - As a result, it wills start trashing the state that A() saved to the stack.

The easy fix is to use a private stack.  you can kmalloc this and pass
it in, or use the constant  STACK_ADDR2.

I should have also simplified other parts of the code so there were less
moving parts and, thus, less things to break.  In particular:



  1. Use the `cps` instruction to set things to user mode (`cps #USER_MODE`) 
     --- this means you don't have to pass stuff in to your trampolines.
     It is also faster since you don't have to do a read-modify-write (mrs/msr).
  2. Have your system call for switching back to SUPER mode do just that --- 
     this means you won't have to pass any paramters in.  It can just set the 
     `spsr` to SUPER and finish.
  3. Not exactly an error, but: i changed my code so that B() returns
     an int indicating whether it finished (1) or could not run yet (0).
     the code just retries until it can run.  this makes the tests much
     simpler.


-----------------------------------------------------------------------------




In this lab we are going to:
  1. extend the last lab so you can single-step through instructions.
  2. Use this to write a concurrency checker that is tiny but mighty.

In many tools, we'd want to run a single instruction and then get control
back for a tool.

In the simplistic memory checker we built, we had the problem that it was
tricky to figure out where an instuction would wind up if if we executed a
single instruction.  Even an `add` instruction can jump somewhere else,
if it modifies the program counter `r15`.  We could handle this (the CPU
certainly does), but there are enough ARM instructions that it becomes
more and more tricky.

-----------------------------------------------------------------------------
### Part 1: Single-stepping.

Last time we set breakpoints that would cause an exception when 
the CPU tried to execute an instruction at a given address.  To do this
you configured the "match" field in the breakpoint control register (BCR).

Today we will configure it to do a "mismatch" as well, where you will
get an exception when the CPU tries to execute an instruction at any
address other than the one you specified.

This may sound like a weird, not useful ability.  However, it lets you
do single-stepping in a cute way:
   1. Set a breakpoint on the first address you want to start running.
   2. When you get a breakpoint exception, set a mismatch on the address
      held in `pc` that we were interrupted at.
   3. Jump back to `pc`.
   4. Because `pc` will *not* mismatch, the CPU will run this one instruction.
   5. When it completes and goes to the next instruction, it will get a mismatch
      and jump back to the exception handler.
   6. You then set a mismatch for that pc value and continue.

While conceptually not hard, there are a couple of ARM-details we need
to deal with:
   1. While we can use breakpoints in "priviledged" mode, the ARM only
      allows mismatches in user mode.  Naively, in order to run at user-level
      it would seem we need to have virtual memory and possibly some notion of
      processes.  However, it turns out (crazy weirdly)  that the ARM allows
      you to run at user-level *without* virtual memory!  (I've never heard
      of such a thing.  Pretty cool.)

      So, after we enable a mismatch breakpoint, before we can take an
      exception we will need to switch the "current process status register"
      (CPSR) from `SUPER` mode (where we start) to `USER` mode.

   2. As you may recall, each mode on the ARM has a private stack
      pointer register (this is why we needed to load one when you
      took an interrupt).  Thus, if we switch to `USER` mode in C code,
      there is a good chance the code crashes or behaves weirdly since
      the compiler will blithely start using the uninitialized user-level
      stack pointer (`sp`) register.  Thus, we need to do this switch
      in assembly.  Since we aren't really running user code, we can
      simply copy the super-level `sp` into a general purpose register
      (so it will be visible to the user-level code), switch modes,
      and then copy that register back into the user-level `sp`.

   3. As we saw in the last lab, when you change privileged state, you
      often have to do some kind of manual flushing.  Changing the `cpsr`
      with the `msr` instruction is no different: when you do this,
      the instructions in the ARM's prefetch buffer have already been
      checked against the debugging values of `cp14` and so will not
      trigger mismatch exceptions when we switch mode.  So you need to
      flush the prefetch buffer.  (You should verify this --- I spent
      20 minutes last night before I realized what was going on.)

   4. When you are at user-level you *cannot* change the mode (this would
      be a security problem).  For this part, we'll simply exit so we don't
      have to worry about it.  In the next part you'll make a system call to 
      do this.  See lab 7 in cs140e if you don't remember how to do this.

To keep things simple, we'll just write a simple trampoline in assembly 
`user_trampoline_no_ret` that just switches mode, jumps to the routine
passed in and does not return:

   - Copy the `sp` into a general-purpose register (so we can see it at user-level).
   - Switch modes (using the new mode in `r0`.
   - Prefetch flush, otherwise you won't get mismatches until later.
   - Setup the sp.
   - Do a branch and link on `r1` 

Tests:
    - `16-part1-test1.c`
    - `16-part1-test2.c`
    - `16-part1-test3.c`

-----------------------------------------------------------------------------
### Part 2: Switching back to supervisor mode

To keep things simple, we'll just write a simple trampoline in assembly 
`user_trampoline_ret` that behaves just like `user_trampoline_no_ret` except
that it:
   1. Allows a return from the trampoline by changing our mode from user
      to back to super.  
   2. Makes the next part a bit easier by passing a pointer to the
      called routine.

As mentioned above, the main complication is that we cannot change the 
current `cpsr` at user level.  To do this you'll have to make a simple
system call.  Recall from lab 7 of cs140e how we make a system call:
   1. To make a system call with 1, you write `swi 1` in the assembly
      (to do 2, you'd write `swi 2`, etc).
   2. In your `software_interrupt_asm` routine you have to save all caller-registers
      adjust the lr, and call a C routine.  Unlike other exception handlers,
      you should *not* load the stack pointer since it is shared with SUPER mode.
   3. In your C code, you should dereference the pc value to see the actual
      instruction and see which constant (in our case, it should be 1).
   4. Change the *spsr* (not the `cpsr` since that will just change the 
      value in the exception handler) to the new value.

As an intermediate step, I would say you should first make a "hello world"
system call that just prints and returns.

For the `user_trampoline_ret` assembly:
   1. You'll have to save the old mode.
   2. Before calling the handler don't forget to move the r2 register to the 
      first argument register (r0).
   3. When the branch and link returns you'll have to call your new system
      call to change back to the old mode.
   4. Return.

Note: the register-based indirect jump seems to be enough to cause
prefetching, so perhaps you could argue we don't need the flush.  I think
there are cases where we could have problems if we ran this code more
than once and it remembered the destination.))

The test `16-part2-test1.c` should pass (sorry there are not more!).

-----------------------------------------------------------------------------
### Part 3: Write a simple concurrency checker.


-----------------------------------------------------------------------------
### Extensions
  1. Don't switch until there was a load or store (can't matter otherwise).
  2. Compute the read-write set from loads and stores so you know when 
     you reach a fixed point.
  3. Have a state hash so you know when you don't have to explore further.
  4. Make a way to yield back and forth.
  5. Write an LDEX lock implementation and check it.
  6. check system call locking.

- use lock with a non-volatile to show is broken.
- Extend to check for null as well as mismatch.

    very cool.
