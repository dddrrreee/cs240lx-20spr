### Detecting magnet contact with a hall effect sensor
	
For this lab you're going to implement the sensor driver on your own
for a simple a simple magnetic sensing device (a "hall effect" sensor).
You'll build a simple fake-pi implementation to cross-check your
code against everyone else.  If you took cs140e, you've used something
like this, but this quarter we'll have you build your own.


<table><tr><td>
  <img src="images/hall.jpg"/>
</td></tr></table>

There's some simple skeleton code, but you're going to do all the work
yourself, including getting the data sheets and anything else you need.
This process will foreshadow what you'll have to do when you write drivers
for devices on your own (with the simplification that in these two cases
we know its possible to get the needed information!).

### Check-off

 1. Show that your hall effect sensor works.
 2. Run your code using the fake implementation and cross-check against everyone. 
 3. They are really cheap, so hook up 3-4 of them, and use them to control some
    number of LEDs.

----------------------------------------------------------------------
#### Part 1: Implementing code for the hall effect Allegro A1104 sensor

First, find the data sheet.   Google search for 'allegro A1104 datasheet'
or some variation of that.  You're looking for the manufacturer's doc
for what to do to use the sensor.

When you find it, you'll notice it's fairly confusing.  This is the norm.
The game is to skim around ignoring the vast majority of information
(which will be irrelevant to you) and find the few bits of information
you need:

  1. What the pins do (input power, output, ground).
  2. What voltage you need for power (`Vcc`).
  3. What reading you get when the device is closed / signaling (`Vout`).

If any of these values are out of range of the pi, you can't connect
directly.

Also, you need to figure out what reading you get when the device is
"open" (not signaling anything) --- in this open state devices are
often not guaranteed to have stable, well-defined output signals.
Thus, you will have to "pull" the signal in the right direction to
make sure it stays there.  If it's high you need a "pullup" resistor
(to pull it high), if low a "pulldown".  We give you routines to do so
(`gpio_set_pulldown(pin)` or `gpio_set_pullup(pin)`).  You call these
after configuring for input or output.

Now, we write the code.
   1. Write the code to configure the GPIO pins.
   2. If you didn't already, implement your `gpio_read` function to read the value
      from a pin.  This will mirror your other `gpio` functions.
   3. Test with a magnet.

Troubleshooting:
  1. Check wiring.   Check if sensor is reversed.
  2. Where you touch the sensor matters.
  3. The magnet orientation matters.
  4. The input voltage matters.

----------------------------------------------------------------------
#### Part 2: Cross-checking your implementation.

TLDR:
   1. Type `make fake`.
   2. It should produce `fake-hall`.
   3. Run it and compare the output.

So far you've been checking each function you write in isolation.
This works because as far as the code is concerned, the only connection
it has to the r/pi is the reads and writes it performs through `put32`
and `get32`.    Thus, by simply providing fake implementations we can
"virtualize" or "emulate" the pi well enough to test the code.

We'll now raise the level on how we fake the pi so that you can easily
cross-check entire pi programs against everyone else.  An additional
benefit of having a small emulator is that since you can easily recompile
your pi programs and run them on your laptop, you can easily find memory
corruption with `valgrind` (or even simple memory protection) and debug
them using  `gdb`.

Note, we could have simply kept using your `put32`/`get32` and just
recompiled more and more of the code in `libpi` that used them.  This is
a good end-to-end test of everything (and we will do it later).  However,
it makes any differences between you and other people harder to figure out
since you have to figure out from a differing `put/get` call which routine
was doing it?  Why did the routine do a different call?  Instead we will
provide higher level fake implementation so its clearer what is going on.

You should look at the code in `libpi-fake` to get a feel for how we do it.
The basic approach is simple: 
   1. We compile your pi program on Unix.
   2. The only question: If your program calls a routine `foo`, what do we do?

A few choices:
   1. Nothing.  For example, we aaren't checking the `uart_init()` code, so our
      fake version simply prints out that it was called and returns.

   2. Call the underlying Unix code.  For example, for `printk` we simply call `printf`.  

   3. Do something fake but reasonable.  When we
      interact with the hardware or with time, we may have to make
      reasonable fake decisions.  E.g., `gpio_read(pin)` reads the value
      of `pin`: we can randomly return a 0 or 1, or bias it more one
      way or the other (or, later, do fancier things).  Time is harder.
      We would simply increment a counter.  Unfortunately that might
      not explore the case when the time counter of the pi wraps around.
      It can also take too long.  So we instead allow the client to set
      time to some maximum step `N`, and each time `timer_get_usec()` is called,
      we advance the currenty time by a random number in `[0..N)`.
   
In general, when making fake implementations, we  often can't explore
all possible outcomes, so have to make a random set of choices.

While trivial to implement in such a limited domain, the basic idea is
powerful, and used in a wide set of areas ranging from hardware companies
validating their chips using existing code up to virtual machines (such as
VMware) faking hardware well enough that it could run multiple operating
systems at the same time as unprivileged processes.

We will keep using it to check your code, but in an increasingly higher
way.  Weirdly, people don't generally seem to do this.  Because we do,
you will miss many of the bugs that mess with their lives.
