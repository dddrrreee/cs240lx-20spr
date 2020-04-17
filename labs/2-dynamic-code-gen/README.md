## Lab: dynamic code generation

*** Make sure you read the [PRELAB](./PRELAB.md) for context. ***

Generating machine code is fun, kind of sleazy, and most people have
no idea how to do it.  So it makes sense as a lab for our class.

As discussed in the `PRELAB` its used for many things, from the original
VMware, machine simulators (closely related!), JIT compilers you may
well use everyday, and even nested functions in gcc.

A second nice thing about generating machine code is that it really
gives you a feel for how the machine actually operates.  When you do
today's lab you will realize that --- surprisingly --- assembly code is
actually high-level in many ways(!) since the assembler and linker do many
tricky things for you.  (E.g., linking jumps to their destinations.)
You will implement a bunch of these things (next lab).

The theory behind today's lab is that we'll do concrete examples of 
a bunch of different things so that it removes a bunch of the mystery.
Subsequent lab(s?) will go more in depth, where you can build out the things
you do today.

The code you'll hack on today is in `code/unix-side`.  Feel free to
refactor it to make it more clean and elegant.    I ran out of time to
iterate over it, unfortunately.
The key files:
  - `check-encodings.c` this has the code you'll write and modify
    for today's lab.  Just start in `main` and build anything it
    needs.

  - `armv6-insts.h`: this has the `enum` and functions needed to encode
    instructions.  You'll build this out.

  - `code-gen.[ch]`: this has the runtime system needed to generate code.
    You'll build this out next time.


If you look at `main` you'll see five parts to build --- each is pretty
simple, but it gives you a feel for how the more general tricks are played:

  0. Define the environment variable `CS240LX_2021_PATH` to be where your
     repo is.  For example:
        
        setenv CS240LX_2021_PATH /home/engler/class/cs240lx-20spr/


  1. Write the code (`insts_emit`) to emit strings of assembly 
     and read in machine code.  You'll call out to the ARM cross compiler
     linker etc on your system.  `make.sh` has some examples.
  2. Use (1) to implement a simple cross-checker that takes a
     machine code value and cross checks it against what is produced
     by the assembler.
  3. Implement a single encoder `arm_add` and use (2) to cross-check it.
  4. Finish building out the code in `derive_op_rrr` to reverse engineer
     three register operations.  It shows how to determine the encoding 
     for `dst` --- you will have to do `src1` and `src2`.  You should
     pull your generated function back in and cross check it.
  5. Do a few other instructions!

Congratulations, you have the `hello world` version of a bunch of neat
tricks.  We will build these out more next lab and use them.

### Part 0: add executable headers in a backwards compatible way.

Even a the ability to stick a tiny bit of executable code in a random
place can give you a nice way to solve problems.  As you might recall,
in cs140e, whenever we wanted to add a header to our pi binaries, we
had to hack

### Part 1: write a `hello world`

Generate a routine to call `printk("hello world\n")` and return.
  - `code/1-hello` has the starter code.

Note: to help figure things out, you should:
  1. Write a C routine to call `printk` statically and return.
  2. Look at the disassembly.
  3. Implement the code to generate those functions.  I would suggest
     by using the cheating approach in `code/examples`.  
  4. To make it really simple, you might want to write exactly those
     values you get from disassembly to make sure everything is working
     as you expect.


You probably want to check your code using something like:

    check_one_inst("b label; label: ", arm_b(0,4));
    check_one_inst("bl label; label: ", arm_bl(0,4));
    check_one_inst("label: b label; ", arm_b(0,0));
    check_one_inst("label: bl label; ", arm_bl(0,0));
    check_one_inst("ldr r0, [pc,#0]", arm_ldr(arm_r0, arm_r15, 0));
    check_one_inst("ldr r0, [pc,#256]", arm_ldr(arm_r0, arm_r15, 256));
    check_one_inst("ldr r0, [pc,#-256]", arm_ldr(arm_r0, arm_r15, -256));


Where my routines are:

    // <src_addr> is where the call is and <target_addr> is where you want
    // to go.
    inline uint32_t arm_b(uint32_t src_addr, uint32_t target_addr);
    inline uint32_t arm_bl(uint32_t src_addr, uint32_t target_addr);


### Part 3: make an interrupt dispatch compiler.

You will
  1. Generate the raw calls to a vector of interrupt routines.
  2. Write timing code to see what the speedup is, with and without
     caching.  (Careful!!!)
  3. Write "threading" code that will jump directly from one routine
     to another.
