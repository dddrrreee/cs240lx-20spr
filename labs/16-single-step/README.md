## Lab: single-stepping to make simple, powerful tools

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

For this part, just set up for mismatch, set up for trampoline.

-----------------------------------------------------------------------------
### Part 1: Single-stepping.

Last time we set breakpoints that would cause an exception when 
the CPU tried to execute an instruction at a given address.  To do this
you configured the "match" field in the breakpoint control register (BCR).
You can also indicate you want an exception on "mismatch" --- when the 
CPU tries to execute an instruction at any address other than the one
you specified.

You can use this ability to do single-stepping in the following way:
   1. Set a breakpoint on the first address you want to start running.
   2. When you get a breakpoint exception, set a mismatch on the address
      held in `pc` that we were interrupted at.
   3. Resume at `pc`.
   4. Because `pc` will not mismatch, the CPU will run this one instruction.
   5. When it completes and goes to the next, it will get another mismatch
      and jump back.
   6. Set a mismatch for the next instruction and continue.

Now, while we can use breakpoints in "priviledged" mode, the ARM only
allows mismatches in USER mode.  Naively, it would seem we need to
have virtual memory and possibly some notion of processes to do this.
However, it turns out (weirdly!)  that the ARM allows you to run at
user-level *without* virtual memory!

So, after we enable a mismatch, before we can take an exception we will
need to switch the "current process status register" (CPSR) from SUPER
mode (where we start) to USER mode.

However, there are a couple small complication:
   1. As you may recall, each mode on the ARM has a private stack
      pointer register (this is why we needed to load one when you took
      an interrupt).  Thus, if we switch modes in C code, there is a good
      chance the code crashes or behaves weirdly since the compiler will
      blithely start using the uninitialized USER `sp` register.  Thus, we
      need to do this switch in assembly.   Since we aren't really running
      user code, we can simply copy the SUPER `sp` into a general purpose
      register, switch modes, and then copy that register back into `sp`.

   2. As we saw in the last lab, when you change privileged state, you
      often have to do some kind of manual flushing.  Changing the 
      CPSR with the `msr` instruction is no different: when you do this,
      the instructions in the ARM's prefetch buffer have already been
      checked against the debugging values of `cp14` (in our case,
      mismatch) and so you will only get a mismatch later, not as soon
      as you switch, so you need a prefetch buffer.  (You should verify
      this --- I spent 20 minutes last night before I realized what was
      going on.)

To keep things simple, we'll just write a simple trampoline in assembly 
that:
   1. Copies the `sp` into a caller-saved register.
   2. Switches mode.
   3. Moves the copied sp into `sp`.
   4. Does a prefetch flush.
   5. Does a branch and link on `r1` (note: this register-based 
      indirect jump seems to be enough to cause prefetching, so perhaps you could argue we don't need the flush.  I think there are cases where we could have problems
      if we ran this code more than once and it remembered the destination.)) 

The tests ... should pass.

-----------------------------------------------------------------------------
### Part 2: Write a simple concurrency checker.

probably the first thing: make it so it can return.

- get rwset so you know when you have reached a fixed point.
- ah: could get the state hash so you have this too.   make a simple 
  model checking lab?

- make sure that panic works.
- check push/pop.
- check push/push
- Have them write an LDEX lock implementation.
- chekc push/push with a lock.

- use lock with a non-volatile to show is broken.
- Extend to check for null as well as mismatch.

    very cool.
