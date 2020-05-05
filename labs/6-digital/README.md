## Lab: digital devices

***NOTE: unless it's very clear to do otherwise, only connect these sensors to 3v, not 5v***
***NOTE: unless it's very clear to do otherwise, only connect these sensors to 3v, not 5v***

Today we'll implement:
  1. Routines to activate the pi's pullup and pulldown resistors.
  2. A simple fake libpi that you can use to run your pi on your laptop and cross check
     its results against everyone else.


As in CS140e, a goal of this course is that you will write every single
line of (interesting) low level code you use.  On the plus side, you
will understand everything and, unlike most OS courses, there will not be
a lot of opaque, magical code that you have no insight into, other than
sense of unease that it does important stuff beyond your ken.  However, an
obvious potential downside of writing the low-level code that everything
depends on: a single bug in it can make the rest of the quarter miserable.

This lab will have you build a simple but complete fake pi system
that will let you automatically check that your pi code is correct.
We will use the approach to check that both your GPIO and your bootloader
implementations  are equivalant to everyone else's implementation.

The basic idea: After completing the lab you will:

    1. Be able to check that your `gpio_pullup` and `gpio_pulldown`
    code is equivalant to everyone else's in the class by tracing all
    reads and writes it does and comparing them to everyone else's
    implementation.  If even one person gets it right, then showing
    equivalance means you got it right too.  And, nicely, automatically
    detect if any subsequent modifications you do break the code (e.g.,
    if you rewrite it to be cleaner).

    2. Understand how to extend your approach so that we can always 
    recompile subsequent labs using your fake system and cross-check
    against everyone.

Embedded systems have the nasty feature that errors can be extremely
hard to detect (e.g., imagine what happens if you slightly miss a
nanosecond deadline or, since we are running with out memory protection,
read or write address 0 on the pi).  On the other hand, bare-metal has
the nice feature that we control everything and the code we write tends
to be self-contained.  In practice we can often easily characterize its
complete behavior by simply tracing its loads and stores, which gives
us a novel tool for checking code equivalance.  Weirdly, I don't know
any software people that exploit this (hardware people do, but only at
the hardware level): hopefully your knowledge of this trick will give
you a secret weapon later.

PRELAB:
  - [How to use a multimeter](https://learn.sparkfun.com/tutorials/how-to-use-a-multimeter/all).
  - [How to use a breadboard](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard/all).

The lab writeup:
  - [0-pullup](0-pullup/README.md)

Checkoff:
   - Show you get the same checksums as everyone else.
