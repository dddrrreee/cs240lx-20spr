### Implement `gpio_set_pullup` and `gpio_set_pulldown`

Checksums:
    % cksum on.out
    4055435488 1074 on.out
    % cksum off.out
    3650815126 812 off.out

    # to run yours
    % ./on.out | cksum
    % ./off.out | cksum


In cs140e we largely ignored an important issue: when many devices
are "open" (not signaling anything) they do not guarantee a stable,
well-defined output signal.  Thus, you will have to "pull" the signal
in the right direction to make sure it stays there when the device is
not signalling anything.  If the open state is high you need a "pullup"
resistor (to pull it high), if low a "pulldown".  Before doing anything
else, we'll build these.

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
the general view is
that too-much is much better than too-little, so I'd go with 150usec.

For what to write:  from looking at both Linux and dwelch67 it *seems*
the broadcom document means to convey that after steps (1)-(4) at set up,
you then do step (6), disabling the clock, but do not do step (5).

If you get confused, [this
page](http://what-when-how.com/Tutorial/topic-334jc9v/Raspberry-Pi-Hardware-Reference-126.html)
gives an easier-to-follow example than the broadcom.


### Check your hardware / implementation with a multimeter.

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
  0. Run `on.bin` Note: the pullup and pulldown are sticky!  You should
     reset them if you don't want them to persist using `off.bin`.
     I think pulldown is generally safe, but pullup can cause issues.

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

     Finally, you should check the unset pin gives you a different result.

  3. Pulldown: this connects the GPIO pin to ground.  Thus, if we measure
     from power to this pin, we should see around 3v.  To do so,
     touch the red multimeter lead to power and the black to this pin.
     Unfortunately, we also get this reading for pins that are in their
     default state, so I'm not sure how to measure that the pulldown
     worked.

When your code works:

  1. Move it to our library:

        `mv gpio-pud.c ../../../../lib240lx/`

  2. Recompile:

        make clean
        make

  3. And make sure it still works.
