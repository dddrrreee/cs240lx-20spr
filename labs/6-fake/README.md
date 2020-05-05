## Lab: digital devices

***NOTE: unless it's very clear to do otherwise, only connect these sensors to 3v, not 5v***
***NOTE: unless it's very clear to do otherwise, only connect these sensors to 3v, not 5v***

PRELAB:
  - [How to use a multimeter](https://learn.sparkfun.com/tutorials/how-to-use-a-multimeter/all).
  - [How to use a breadboard](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard/all).

Today you'll build two things:
  1. A set of low-level pi routines that can be used to keep devices in a
     "clean" state (by configuring GPIO pins to connect to "pullup" and
     "pulldown" resistors).

  2. A simple fake libpi that you can use to run your pi on your laptop
     and cross check its results against everyone else.

In this class and cs140e we largely ignored an important issue: many
devices when "open" (not signaling anything) do not guarantee a stable,
well-defined output signal.  Thus, you will have to "pull" the signal
in the right direction to make sure it stays there when the device not
actively signally anything.  (Otherwise environmental noise can cause
phantom signals.)  If the open state is high you need a "pullup" resistor
(to pull it high --- in our case to 3.3v), if low a "pulldown" (to pull it
to ground).  In Part 1 of this lab you will build these routines.  (To be
honest, we should have had them for all of this quarter and cs140e.)

A goal of this class is that you write every line of (interesting) low
level code you use.  On the plus side, you will understand everything and,
unlike most OS courses, there will not be a lot of opaque, magical code
that your only insight into is a sense of unease that it does "important
stuff."  However, an obvious downside of writing the code that everything
depends on: a single bug in it can make the rest of the quarter miserable.

To attack this problem, this lab will have you build a simple but working
fake pi system that will let you automatically check the behavior of
your pi code against everyone else's.  We'll extend the fakeness as the
quarter goes on, letting you auto-check increasingly fancy pi stuff.
After completing the lab you will:

    1. Be able to check that your `gpio_pullup`, `gpio_pulldown` and
    `gpio_pud_off` routines are equivalant to everyone else's in the
    class by tracing all loads and stores it does and comparing this
    memory trace them to everyone else's implementation.  If two pieces
    read and write the same memory locations in the same order with the
    same values, they must be equivalant (at least on the tested inputs).
    If even one person gets the code right, then showing equivalance
    means you got it right too.

    In addition to making it easy to check across people, memory tracing
    also makes it easy to check across time: you can use it to check your
    own code against itself and automatically detect if any subsequent
    modifications you do breaks it (e.g., if you rewrite it to be cleaner
    and delete a needed statement).

    2. Start to understand how to emulate systems in a fake but useful way
    --- your system won't be that sophisticated, but is useful in that it
    gives a complete, concrete "hello world" level example of emulating
    one system on another, and the benefits from doing so.  Later you'll
    extend your approach so that we can always recompile subsequent labs
    using your fake system and cross-check against everyone.

The lab has four parts:
  1. Implement pullup-pull down.
  2. Check your implementation using a multi-meter.
  3. Implement `libpi-fake` and check your implementation of (1).
  4. As a final step, move your implementation to the CS240LX libpi.

Checkoff:
   - Show you get the same checksums as everyone else.

-------------------------------------------------------------------------------
### Part 1: Implement `gpio_set_pullup` and `gpio_set_pulldown`

These are described, with much ambiguity, in the Broadcom document on
page 101 (using my pdf reader page numbering).  Some issues:

  1. They say to use clock delays, but do not way which clock (the pi clock?
  The GPU clock?  Some other clock?)

  2. A straight-forward reading of steps (5) and (6) imply you have
  to write to the `GPPUD` to and `GPPUDCLK` after setup to signal
  you are done setting up.  Two problems: (1) write what value? (2)
  the only values you could write to `GPPUD`, will either disable the
  pull-up/pull-down or either repeat what you did, or flip it.

In other domains, you don't use other people's implementation to make
legal decisions about what acts are correct, but when dealing with
devices, we may not have a choice (though, in this case: what could we
do in terms of measurements?).

Two places you can often look for the pi:

  1. Linux source.  On one hand: the code may be battle-tested, or
  written with back-channel knowledge.  On the other hand:
  it will have lots of extra Linux puke everywhere, and linux is far from not-buggy.

  2, `dwelch76` code, which tends to be simple, but does things (such as
  eliding memory barriers) that the documents explicitly say are wrong
  (and has burned me in the past).

For delays:
[Linux]
(https://elixir.bootlin.com/linux/v4.8/source/drivers/pinctrl/bcm/pinctrl-bcm2835.c#L898) uses 150 usec.  [dwelch76]
(https://github.com/dwelch67/raspberrypi/blob/master/uart01/uart01.c)
uses something that is 2-3x 150 pi system clock cycles.  The
the general view is that we are simply giving the hardware "enough" time to settling
into a new state rather than meeting some kind of deadline and, as a result,
that too-much is much better than too-little, so I'd go with 150usec.

For what to write:  from looking at both Linux and dwelch67 it *seems*
that after steps (1)-(4) at set up,
you then do step (6), disabling the clock, but do not do step (5) since its simply 
hygenic.   

Since we are setting "sticky" state in the hardware and mistakes lead to non-determinant
results, this is one case where I think it makes sense to be pedantic with memory barriers:
do them at the start and end of the routine.  (NOTE, strictly speaking: if we are using 
the timer peripheral I think we need to use them there too, though I did not.  This 
is a place where we should perhaps switch to using the cycle counter.)

If you get confused, [this
page](http://what-when-how.com/Tutorial/topic-334jc9v/Raspberry-Pi-Hardware-Reference-126.html)
gives an easier-to-follow example than the broadcom.


-------------------------------------------------------------------------------
### Part 2: Check your hardware / implementation with a multimeter.

With hardware we want to sanity check as much as possible.  This both:
  1. Detects hardware issues (common: mis-wiring, not-unusual: broken components), and:
  2. Pushes our understanding farther, much in the same way you understand a theorem or 
     measured result more when you derive it in multiple ways.  My tentative theorem
     here is that if you don't see how to check a circuit using measurments or cross-checking
     you likely don't understand it.  (I am often guilty of this, which is why it is good
     we have Brian in the class.)

So, while turning on GPIO ouput/input gives an obvious way to check in
software, setting pullup-pulldown is less clear, with the added bonus
that the words describing it in the Broadcom are super unclear.

So, test things with a multimeter:
  0. Run `on.bin` 

  1. Measure the resistance of some unset pins: , I got about 50k
     resisance (for our multimeters you need to have the dial set
     at 200k).  Make sure they give you about the same value or that
     could show a pi defect!  (But, most likely it is showing a defect
     in how you hold the probes.)

  2. Pullup: this connects the GPIO pin to 3.3v in the "open", default
     state.  Thus, if we measaure from this pin to ground we should
     get around 3v.  You can do this measurement by touching the red
     multimeter led to the pin (in the breadboard, ideally) and the
     black to ground.

     As a second method, you can plug in an LED --- long leg connected
     to the pin, short to ground.  It should light up.  Though, it will
     be much dimmer than if you connect to 3v directly: why?

     Finally, you should check the unset pin (after you run `off.bin`)
     gives you a different result.

  3. Pulldown: this connects the GPIO pin to ground.  Thus, if we measure
     from power to this pin, we should see around 3v.  To do so,
     touch the red multimeter lead to power and the black to this pin.
     Unfortunately, we also get this reading for pins that are in their
     default state, so I'm not sure how to measure that the pulldown
     worked.  (If you figure something out, please let me know!)

Note, to repeat: the pullup and pulldown are sticky!  You should reset
them if you don't want them to persist using `off.bin`.  I think pulldown
is generally safe, but pullup can cause issues.

-------------------------------------------------------------------------------
### Part 3: implement `libpi-fake` and crosscheck your code.

We want to cross-check your code against everyone to detect differences.
We could do `strcmp`, but since everyone's code is different, that
would not be useful (other than in a MOSS-context).  We take a different
approach and simply trace the side-effects the code does.  
If we play the game right, code that does the same side-effects in
the same order will have equivalant behavior and code that does not,
will not.  (For now, we do not worry about handling spurious differences,
such as the order that pins are made input or output; this may be done in
a later lab.)  These could be low level side-effects (such as `PUT32` and
`GET32`) or higher level (such as `gpio_set_input`, `rpi_reboot`). We
consider two pieces of code *trace equivalant* if they have the same
trace.  We also consider that a successful lab chec-koff!

Today you will build a simple but useful cross-chedcking system.  It has two 
parts:

  1. Getting your pi code to run on Unix on your laptop.  This involves
     replacing different pi functions with our own versions.

  2. Tracing what your pi code did.  For the most part, we simply print
     out each pi routine called (possibly with extra information).

     If your code *does not* visit the same routines in the same order
     as someone else then it's probable (but not guaranteed!) that
     does not compute the same results.   Of course, since we do not
     perfectly simulate the pi, trace equivalence cannot guarantee
     actual equivalences (much less correctness), however it is often
     significantly stronger than just running a few tests and saying
     "ship it".  The `driver.c` code can do as many random runs as you
     like, which can somewhat mitigate this problem.


An additional benefit of running tests on Unix is that it has memory
protection, so if you do an invalid memory access there is some chance you
will detect this with a crash.  We do not get anything similar on the pi
for a few more weeks --- our pi setup will not even catch a dereference
of null now, as you can check:


    // trivial pi program that reads and writes using the null
    // pointer.
    void notmain() { 
        volatile unsigned *null = (void*)0;
        // writing to null -- try this on Unix!
        *null = 0xfeedface;
        printk("*NULL = %x!\n", *null);
    }


#### Making a high-level libpi-fake

We'll first make a "high-level" version of your fake pi library:

  1.  Do `make fake` in today's lab's `code/` directory the
      `Makefile` will compile fake versions of `on.c` and `off.c`.

  2. Initially the linking phase will fail with a bunch of undefined
     references (`printk`, `gpio_set_input`, etc.).  These are exactly
     the routines you will have to implement on Unix.

  3. Put fake vesions of each undefined routine in
     the directory `libpi-fake` (at the top of the class directory).
     Write your routines so they use the provided `trace` macro to print
     identical messages to the output provided in the provided files
     `on.out` and `off.out`.   Each routine should be in a new file ---
     do not put them in the `fake-pi.h` header nor in any of the existing
     files.  The (linker-specific) reason for this will be clearer later,
     but for the moment, we want to put these routines in their own file.
     You do not have to modify the `Makefile`,
     it will compile the new files automatically.

  4. When compilation succeeds, it will produce `on.fake` and
     `off.fake`.  If you run `on.fake` you should get the same output as
     `on.out` (`./on.fake | diff on.out -`).  Similarly for `off.fake`.

After you pass: congratulations!  You now have a working emulation system.
You can push this direction pretty far.  Hardware implementors do all
the way down to bit-accurate simulation of registers, TLBs, memory,
caches, etc.  We will extend our version somewhat, but even in its
simplistic form is enough to detect a bunch of different, nasty errors
such as missed memory barrier and writes to incorrect addresses.

#### Making a low-level libpi-fake

We now show how with almost no effort we can make your checking system 
more thorough by, instead of using fake routines (as above), simply 
pull in and compiling the real `libpi` versions.  

Intuitively, while we cannot recompile ARM assembly language on your
likely-x86 laptop, for much of the C code in `libpi` there is no real
difference between running it on the pi or on Unix --- a `for` loop,
`if`-statement, assignment, etc have perfectly well-defined semantics on
both systems.    Since we were careful to to never dereference pi-specific
addresses directly but instead use `PUT32` and `GET32` running the code
is a matter of producing "reasonable" output for these routines.

This discussion is easier to understand by just working through 
the lab.  As our first step,  modify the `lipi-fake/Makefile` and 
change the variable: 

    RAW_PI_SOURCE = $(LPI)/cs140e-src/timer.c


This will cause the `make` process to pull in the raw `timer.c` code
from our actual `lipi`.  If you do `make fake` again in the lab's `code`
directory (as in the previous step).  This will produce a `on.fake` and
an `off.fake` that call the timer code directly.  Because of how the
`lipi-fake/Makefile` was written, your `on.fake` should link against
the raw `timer.c` rather than your stubbed out version.
However, when you run you'll get an unimplemented error because 
your code calls `timer_get_usec`, which in turn calls:

    // no dev barrier.
    unsigned timer_get_usec_raw(void) {
        return GET32(0x20003004);
    }



Well what should we do?  We got away with not thinking hard to fake
`gpio_set_pulldown` and `gpio_set_pullup` since these routines were
write-only: they only did `PUT32` and not `GET32` so we could simply emit
their writes.  To support `GET32` we need to do something more sensible.

If we want to emulate normal memory semantics, we could return the
value of the last write.   We could even be lazier, and just return a
random value --- we are just checking for equivalance, not for strict
correctness, and in many cases random can work as a first cut.  For the
specific case of the time address we will make a small effort to be
"reasonable":

  1. In `GET32` check if the address is equal to `0x20003004` and if so,
     return a monotonically increasing counter each time the address
     is read.  For today's lab, just add a `1` to the fake time variable
     `fake_time_usec`.

  2. Otherwise return a random value.  Use `fake_random()` so that we
     can cross-check against everyone else.

Such special casing of different device addresses can be pushed to an
extreme degree, as in the case of classic virtual machines, which would
emulate an entire device such as a SCSI disk or an ethernet card.  We
will spend much less energy, but the basic idea isn't dissimilar.

To check your code:
  1. Recompile by doing `make fake` and running `on.fake` and `off.fake`.
  2. Your results should match `on.timer.out` and `off.timer.out`.

Finally, we will pull in your `gpio.c` implementation as well:

  1. Add your GPIO path to `RAW_PI_PATH`.  For example:

        RAW_PI_SOURCE = $(LPI)/cs140e-src/timer.c $(LPI)/gpio.c 

  2. If you used `PUT32` and `GET32` in your `gpio.c` then running `make fake` should just 
     work.  
  3. The results should match: `on.gpio+timer.out` and `off.gpio+timer.out`.


To summarize the cksums:

    cksum on.out
    4055435488 1074 on.out
    cksum off.out
    3650815126 812 off.out

    cksum on.timer.out
    1301434085 50330 on.timer.out
    cksum off.timer.out
    2981280742 50068 off.timer.out

    cksum on.gpio+timer.out
    2860634443 50553 on.gpio+timer.out
    cksum off.gpio+timer.out
    2981280742 50068 off.gpio+timer.out

-------------------------------------------------------------------------------
### Part 4: finalize.

When your code works:

  1. Move it to our library:

        `mv gpio-pud.c ../../../../libpi/`

  2. Recompile:

        make clean
        make

  3. And make sure it still works.

-------------------------------------------------------------------------------
### Discussion

Stepping back a bit: Embedded systems have the nasty feature that errors
can be extremely hard to detect (e.g., imagine what happens if you
slightly miss a nanosecond deadline or, since we are running without
memory protection, read or write address 0 on the pi).  On the other
hand, bare-metal has the nice feature that we control everything and
the code we write tends to be self-contained.  In practice we can often
easily characterize its complete behavior by simply tracing its loads
and stores, which gives us a novel tool for checking code equivalance:
just compare these load and store traces.  Weirdly, I don't know any
software people that exploit this (hardware people do, but only at the
hardware level): hopefully your knowledge of this trick will give you
a secret weapon later.
