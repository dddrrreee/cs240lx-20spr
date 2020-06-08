### simple memory checking using VM faults and single-stepping

This is our final lab!   It's optional, in case people want help getting
things working in previous labs.

If you're in a good space, this is a fun lab.

In lab 10 we did stochastic binary rewriting to attach to load and store
instructions.  That was fun but it had two problems:
   1. We had to rely on sampling to discover instructions.  We have no guarantees
      that we found all of them (and in fact, would not).
   2. Because we moved instructions into a trampoline, we had to have perfect 
      understanding of their semantics in terms of (1) that they did not 
      depend on their location and (2) where they would go when execution 
      finished.   For loads and stores we assumed they would not do anything
      besides fall though, but that's not necessarily the case for loads, 
      and it's definitely not the case for other instructions (e.g., with 
      the pc as an operand).

Today's hack will eliminate both of these problems.  And, now that you have
single stepping, is not that hard!

The basic idea:
   1. We will mark all heap memory as not having either read or write 
      permissions (other areas can work similarly).
   2. As a result, any load or store to a heap location will result in a memory
      fault.
   3. If the memory address of the memory operation is not legal, we will 
      emit an error.   
   4. Otherwise we will continue execution:
   5. WeIn the handler, we will switch permissions for that page (more on this
      right below), perform the load or store, and then restore permissions.


If you think about it, the last step is a little tricky: if we are not
willing to relocate the load or store via a trampoline, how do we get
control after the instruction runs so we can restore permissions?
This is easy: just single-step.  

You have this code; you just need to tweak the fault handler logic
slightly.  It's even easier than the last lab, since you're not trying
to interleave execution between two processes.

The only other question is: how quickly can we change permissions?
By exploiting some custom ARM tricks, it seems we can do this pretty fast:

   1. ARMv6 let's you tag each page with one of sixteen possible *domain ids*.

   2. Importantly, with a single register write, you can simultaneously
      set the permissions bits for all 16 domains.
      For example, whether for domain id `4` if the current process 
      write pages tagged with 4, or read them, or neither.

Thus our checker works as follows:
   1. Set all pages in the heap to a unique domain (e.g., 1).
   2. Set the access for the current process to no read and no write 
      permission to domain 1.
   3. Start running the code in user mode.
   4. If it does not load or store the heap, it runs full speed, just
      as it would if it was not monitored.
   5. When it does a load or a store to a heap address you will get
      a permission fault.

Our fault handler is written as follows.  After we have checked that
we have a data fault, and the address address of the load or store is
legal, we:
   1. Set the single step code to mismatch when we jump back to the 
      faulting instruction (just as you did the last lab).
   2. Change the process's permission for domain 1 to read/write.
   3. Jump back.
   4. The instruction will run.
   5. We will then get a mismatch fault (just as in the last lab).
   6. In the single-step handler: we change permission for domain 1
      back to no-read/no-write.
   7. Jump back.
   
The end result is that you can check if *every* load or store is to a
legal chunk of memory using a few hundred lines of code.  You can compare
this the million+ lines needed by a valgrind based on binary rewriting.
Single-stepping is insanely cool!

The open question left is how to set permissions.  In the interests of
time I've given you a slightly altered virtual memory implementation
from the cs140e code base.  You can likely use your own, but for the
moment stick with the stack one for debugging.  (Though, of course
a bug in my conceptual model last time cost you all hours, so maybe
"trust but verify", at least to the extent you can.)


---------------------------------------------------------------------
### Part 1: just do a heap bounds check.

You're given the code to map the address space, setup the page tables,
and set up the domain.

In this part, check that the faulting base address is within bounds of
some heap object.
   - As a first step, just check  that the access is within the legal heap.

   - After you get control when you change pemissions, if the instruciton
     was a store,  check the redzone within 32 bytes above and below
     the address to see if there was a violation.

---------------------------------------------------------------------
### Part 2: speed using shadow memory

Create a shadow memory map of your heap and check the address against
this.

Compare the cost of running in part 1 and part 2.

---------------------------------------------------------------------
### Part 2: accuracy

Our checking is weak in that we won't detect:
  1. Out of bounds stores that write the same value.
  2. Out of bounds loads that have a valid base, but load multiple registers.
  3. Out of bounds stores that write outside of the intended object 
     but within another legal one.

We have hacks for all three!  TBA :).
