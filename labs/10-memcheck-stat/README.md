## Statistical valgrind

***Make sure you do the [PRELAB](PRELAB.md)!***

Today we will build a simple memory checking tool that can check
if an executing program performs a read or write to invalid memory.
Typically such tools (Valgrind, Purify) are incredibly complex and large
(100K lines of code or more).  Most of their complexity comes from the
fact you cannot reliably statically figure out what is code and what
is data in a program.  As a result, they have to do dynamic binary
instrumentation, which requires the ability to parse and (much harder)
correctly specify the semantics of all binary instructions.

In constrast we will use a new trick to build one that is simple, and
a few hundred lines.

Our tool works as follows:
  1. We discover code not by dynamically tracing the program, but instead
     by using timer interrupts to statistically sample the program binary.
     By definition: any address we interrupt contains an instruction
     (since it was being executed).

  2. We replace each load an store instuction we discover with a jump
     to a custom checking routine for that specific code address.
     Because the jump is the same size as the load or store, the program
     binary does not change size ("dialate") and so we do not have to
     patch program addresses (infeasible if you can't reliably classify
     code and data).  Because the trampoline is customized to that
     specific operation, it is reasonably fast, and can easily return
     control back to the original location if the operation checks out.

  3. We check the state of each memory location using the common method of
     "shadow memory" --- all bytes the program can access have a corresponding
     shadow location that tracks their state (allocated, freed, invalid).
     Shadow memory is allocated at a specific, known offset from the proram's
     memory so that it is fast and simple to look up the state of a
     memory location given just the address.  (A few bounds checks and then 
     an addition.)

There is a lot going on in this lab, so we do a very stripped down,
minimum viable product.  It's not general, it's not rebust, but it will
have `hello world` level examples of the key pieces so that you have
something to think about over the weekend, which will help when we build
these out later.

The lab has four pieces:
   1. Make a simple redzone memory allocator. This will
      catch some number of memory corruptions, and will help prevent us
      from missing if a pointer jumps from one object to another.
   2. Make a simple shadow memory implementation that tracks what
      chunks of memory are allocated, freed, invalid, or read only.
      This will allow us to take any address in the program and figure out
      if it is legal.
   3. Use timer interrupts to make a sampling routine that examines the interrupted
      instruction and, if it is a load or sore, uses the shadow memory in (2)
      to determine if the access is legal.
   4. To make sampling more thorough: when you discover a memory location
      in step (3) instead of checking and returning, we instead
      rewrite the instruction to call a trampoline that will check it.
      This means that when we interrupt a load or store once, the check
      will be performed for all future executions.  I.e., it is sticky,
      or persistent.

Checkoff:
  1. Showing you find the errors in the checked in programs.
  2. Adding a new buggy program to Peter's contrib repository so that 
     we can extend things.

### Part 0: some new functionality.

I put a new version of libpi in the top of our repository --- it was
getting a bit weird to modify the cs140e code.   You can put any code
you want to use in `libpi/src` and modify `put-your-src-here.mk`.
    - The example in `examples/hello` should run if you compile it.

I also set it up so you can use floating point.  
    - The example in `examples/float` should run if you compile it.

Its a hack where we compile `libpi` in two different ways (with float
and without).   The linker should give you an error if you link against
the wrong one.

### Part 1: redzone allocation.
### Part 2: shadow memory
### Part 3: statistical sampling
### Part 4: injecting trampolines.
