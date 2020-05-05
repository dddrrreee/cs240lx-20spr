## ****TENTATIVE*** labs.

These are a ***tentative*** set of labs.  They will change!  Some will
get dropped.  Some will get added.  All will get modified significantly
as we get a feeling for what has to change to teach a lab class remotely.

In addition, this is the first offering of the class, and we will be
writing the code right before you do, so the only constant will be change.
If rough edges and flux worry you (natural) it may be better to defer
taking the class.

Note: if you've taken CS140E or CS49N, some of the labs will look similar.
Do not worry --- the labs will have changed somewhat (to get better)
and there will be plenty of extensions for each to keep you busy with
new tricks.

---------------------------------------------------------------------
### Part 1: Compilation tricks

  - [1-trusting-trust](1-trusting-trust): You'll implement a simplified
    version of Ken Thompson's evil compiler
    hack that we described in his Turing award lecture.


  - [2-dynamic-code-gen](2-dynamic-code-gen/README.md): you'll learn
    how to generate executable machine code at runtime and how to 
    use this trick to do neat stuff.

---------------------------------------------------------------------
### Part 2: fancy tricks with timing-accurate code 

  - [3-logic-analyzer](3-logic-analyzer/README.md): the pi is fast enough,
    and flexible enough that we can easily build a logic analyzer that
    (as far as I can tell) is faster --- and certainly more flexible ---
    then commercial logic analyzers that cost hundreds of dollars.
    You'll learn how to write consistently nanosecond accurate code.

  - [4-uart-8n1](4-uart-8n1/README.md): 
    Now that you can write and verify timing-accurate code, we are going
    to use this to make a simple, fast pin-to-pin network on your pi and
    see how efficient you can make it, both in reducing latency and in
    increasing bandwidth.

  - [5-ws2812b](5-ws2812b/README.md) you'll write nanosecond accurate
    code to control a WS2812B light string.  The WS2812B is fairly common,
    fairly cheap.  Each pixel has 8-bits of color (red-green-blue)
    and you can turn on individual lights in the array.

---------------------------------------------------------------------
### Part 3: Devices

   - [6-fake](6-fake/README.md): you'll write low level GPIO code to
     make device output cleaner, and then build a fake libpi that lets you
     run your pi code, unaltered, on UNIX and cross-check its side-effects
     against everyone else.


---------------------------------------------------------------------
---------------------------------------------------------------------
Be warned: the labs below have not been updated.


Additional benefits:
  1. 90% of OS code is device drivers, so it makes sense to learn how
     to write this kind of code.
  2. You will write your own emulator layer so that you can cross check your
     code against everyone else.

#### Digital

We start with the simplest type of devices --- digital (which only turn
on, off) to get situated.

   1. Hall effect.
   2. Color.

#### Time

You can encode more data when you add time to 0 and 1.
We will write the code to control:

  1. A sonar device to measure distance
  2. ws211b light arrays (which require  nanosecond timings).

#### Protocols: i2c and spi

We then switch to using higher level protocols, starting
with the I2C method.  

   1. SPI: very common, simple protocol.  You'll implement it and use
      it to control a digital-to-analogue converter and to control an
      OLED screen.

   2. I2C: more complicated but less wires than SPI; also
      widely-available.
      We'll give you a binary version, which you will use to control
      an accelerometer, a gyroscope and an accelerometer.  Once your
      accelerometer code works, you'll replace our I2C implementation
      with yours.

##### Analog

Analog devices (that output a varying voltage) are typically
the cheapest ones.  Unfornately your r/pi cannot take their input
directly.  We will use your I2C driver to read inputs from a microphone
and control the light array with it.

---------------------------------------------------------------------
### Part 3: Exceptions, threads, and binary tricks

CS240 starts off discussing race-condition detection (Eraser), threading
issues (Bohm) and cooperative vs pre-emptive threads.  These initial
labs give you a much more concrete grasp of these topics.

  - [Interrupts]: we give you several pages of code showing a complete,
    working interrupt system for the pi.  We will go over each line, and
    you will modify the code in various ways to ensure you understand
    it (changing where the exception vectors are, returning from the
    interrupt handler in a cleaner way, changing the code to be faster,
    catching another type of exception, making the timer interrupt much
    more frequent).

  - [Statical valgrind]: Use interrupts to do a statistical memory
    corruption checker.  You will write a simple `kmalloc` implementation
    to use guard zones so that you can (often) tell when a pointer
    wanders from one allocated object to another.  You will also set
    allocated memory to a known pattern so you can detect if a read happens
    before an initializing write.  Set your timing interrupts to be very
    frequent and in the handler, decode each interrupted instruction,
    and see if it is out of bounds, reading uninitialized memory, or is
    writing beyond the end of the stack.  It may miss errors, but will
    be very fast and should do a reasonable job, given a long enough
    run and a fine-enough window.   We will make it more effective later.

  - [Potential]: Lots of people complain about how easy it is to make
    mistakes with cooperative threads.   You will write interrupt-based
    checkers to detect these problems: flag when a thread runs too long
    with interrupts disabled, too long without yielding, in a deadlock,
    spinning on a condition that cannot change, extend past the end of
    the stack, have priority inversion, starvation, missed deadlines,
    lock queues that are too long, or critical sections that are too long.

  - [Lock-free checking] Lock-free data structures (as used above in our
    UART code) are extremely hard to get right.  You will write a tool
    that can check them.

    ARM code is not that hard to parse (at least compared to x86).  We can
    use this ability to make a effective, but hopefully simple lock-free
    algorithm checker.   Given a set of functions that purport to run
    correctly (either with or without locks) we can:
      - disassemble them.
      - at each load or store, insert a call to a context switch.
      - run the code with two threads.   
      - first test that they give the same result as two sequential calls
        if we do a single context switch at each possible point, then two 
        context switches, etc.

    To be really fancy, we can buffer up the stores that are done and
    then try all possible legal orders of them when the other thread
    does a load.  This checker should find a lot of bugs.

    To speed up checking we use state hashing to prune out redundant
    states: at each program counter value, hash all previous stores,
    if we get the same hash as before, we know we will compute the same
    results as a previous and can stop exploring.

------------------------------------------------------------------
### Part 4. Virtual memory, and improving tools.

We'll have to modify a simple virtual memory system to get started and then 
use memory protecton to build tools.

   - [Fast protection domains]: As you probably noticed, we have no
   protection against memory corruption.  As your system gets bigger,
   it becomes much much harder to track down which subsystem is causing
   everything to crash.  Going all the way to UNIX style processes
   is one attack on this problem, but we can use ARM do to a lighter
   weight method:

     1. Give each domain a unique domain number.
     2. Tag all memory associated with the domain with its domain number.
        (you may have to have additional domain numbers for pages accessible
        by more than one).
     3. Before you call into the domain, change the domain register so that
        domain has client privileges, but no other domain is accessible.
     4. At this point it can only touch its own memory.

   This method is much faster than switching between processes: we don't
   have to flush the TLB, caches, or page table.

  - [Trap-based Eraser]: recall that the original Eraser worked by using
    binary-rewriting to instrumenting every load and store and flagging
    if a thread accessed the memory with inconsistent set of locks.
    Doing binary rewriting is hard.  We do the alternative: simply trap
    on every load and store and implement the lockset algorithm.

  - [Trap-based memory checking]: Valgrind dynamically rewrites binaries
    so that it can instruments every load and store.  Dynamically
    rewriting binary is harder in some ways than the static rewriting
    that Eraser does, which was already really hard.  You will use your
    traps to implement a much simpler method:

    - Mark all heap memory as unavailable.
    - In the trap handler, determine if the faulting address is in bounds.
    - If so: do the load or store and return.
    - If not: give an error.
  
    Given how fast our traps are, and how slow valgrind is, your approach
    may actually be faster.

  - [Volatile cross-checking] A common, nasty problem in embedded
    code is that programs code use raw pointers to manipulate device
    memory, but either the programmer does not use `volatile` correctly or
    the compiler has a bug.  We can detect such things with a simple hack:

    - We know that device references should remain the same no matter 
      how the code is compiled.  
    - So compile a piece of code multiple ways: with no optimization, `-O`,
    `-O2`, with fancier flags, etc.  
    - Then run each different version, using ARM domain tricks to trace 
      all address / values that are read and written.  
    - Compare these: any difference signals a bug.  

    This tool would have caught many errors we made when designing cs107e;
    some of them took days to track down.

  - [Thorough conconcurrency checking]: we will use traps to build a simpler, 
    and more aggresive version of the lock-free checking tool above.  
    - Mark thread memory as inaccessible.
    - On each resulting fault, do the instruction but then context switch 
      to another thread.
    - At the end, record the result.
    - Then rerun the thread code from a clean initial state without doing
      context switches.   For the type of code we run: Any difference
      is an error.

------------------------------------------------------------------
#### Part 5. Virtual machines, simplified.

   - [Simple VMM] We will use our trapping virtual memory system and
     pre-emptive threads package to build a simplistic virtual machine
     that can run pi programs as micro-guest OSes (in the sense that they
     run in God mode, albeit implement minor functionality).  You will
     do by mapping them into their own address spaces, trapping all GPIO
     reads and writes and forwarding them.

   - [Using the VMM] Typically we think of VMMs as slowing down execution.
     In this case you will use your VMM to improve the basic pi execution model by:
     0. Preventing silent memory corruption by adding memory protection, 
        catching wild memory accesses.
     1. Making guest programs run much faster by transparently enabling the icache 
        and dcache.
     2. Our original programs spend much of their time blocking waiting
        for the UART to receive characters or make space in its transmit
        buffer (or, worse: dropping bytes).   Your VMM will transparently
        make the UART code interrupt driven, removing these problems.

------------------------------------------------------------------
### Speed

  - [Speed]: Rewrite the interrupt / exception handling to be much
    more efficient and extensible.  Rip the code down to the bare minimum,
    enable icache, dcache, BTB, any other tuning you can.  See how much
    faster you can make it compared to where we started.

    We will run common micro-benchmarks on your laptop and see how
    much you can beat them by on the much-slower pi hardware using the
    much-faster code you wrote.    Example benchmarks: how fast you
    can ping-pong bytes between threads, take a protection fault, etc.
    If you beat linux/macos by 50x I wouldn't be surprised.
