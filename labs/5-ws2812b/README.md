## Controlling a  WS2812B light string.

Today you'll write the code to control a WS2812B light string.
The WS2812B is fairly common, fairly cheap.  Each pixel has 8-bits of
color (red-green-blue) and you can turn on individual lights in the array.

Each light strip has an input direction and an output direction (look
at the arrows).  You can attach multiple in series by connecting the
output of the first one to the input of the second.  They can be cut
into pieces, connected, etc.

The WS2812B's are a good example of how to communicate information
using time.  One way to communicate things fancier than on/off is to use
a fancier protocol (e.g., I2C, SPI).  Another is to just use delays ---
if you have good resolution (low variance, precise control over time and
ability to measure it), each nanosecond of delay can signify information.
The WS2812B's use a crude variant of this.

The WS2812B protocol to send a bit of information:
  1. To send a `1` bit: write a `1` for `T1H` nanoseconds (defined in datasheet), then a `0`
     for T1L nanoseconds.
  2. To send a `0` bit: write a `1` for `T0H` nanoseconds, then a `0` for `T0L` nanoseconds.
  3. To send a byte: send the 7th bit (as above), the 6th, the 5th...
  4. The timings are fairly tight.  The datasheet states +/- 150 nanoseconds.  Thus
     you will have to write your code very carefully.  You will also have to be 
     smart about measuring it --- your measurements take time and will pollute the 
     times you measure.

The protocol to set N pixels:
  1. There will be some number of pixels on each light string.  (You can count them.)
  2. Each pixel needs 3 bytes of information (G, R, B --- in that order) for the 
     color.
  3. Send each byte using the protocol above.
  4. The first 3 bytes you send will get claimed by the first pixel, the second 3
     bytes by the second, etc.

### Check-in


You'll implement several pieces of code:

  1. Timing routines in `code/WS2812.h`.  These should pass the timing
     tests run by `check_timings`.   Since part of this class is learning
     to use datasheets, *please* only use the datasheet we have (in
     `doc/`) and the comments in the given code, don't use blog posts,
     etc.

  2. The simple interface in `neopixel.c` which
     wraps the code you wrote in (1) into a simple interface.  Optimize
     the timings as far as you can and have the code still work.

  3. Do something novel/cute with the light using your interface.

###  Part 0: hook up your hardware.

To hook up the light string:
   1. Strip the red (power) and white (ground) wires and connect them
      to the 3.3v power and ground of the pi.  Unfortunately I didn't realize these
      were stranded wire, so you will have to (sadly) also strip some jumper cables and
      twist-connect them.
   2. Hook the pi's 3.3v and ground up to the female connectors on the light strip as well,
      using male-to-male to go to your breadboard.
   3. Hook up the green (signal) wire to pin 21.
   4. Bootload the programs in the lab's `bin/` and make sure they do something. 

Note: A confusing thing is that the power for the WS2812B must be pretty
close to the power used to drive the data pin.  For long light strings
you'd want to power them with a external power supply --- however this
will require you either use a "level shifter" to raise the power used
by the pi to signal (since its pins are 3.3v for output) or a transitor
to connect/disconnect the pi 5v to the signal wire of the LED.

### Part 1: implement the timing routines  (30 minutes)

The header `code/WS2812b.h` defines routines to call to write a 0, write a 1,
write a pixel to the light array.  It also defines timing constants.
You should fill theese in using the datasheet and comments.  When you
are done, they should pass the timing checks called by `code/simple.c`.

These routines will look similar to the other timing routines you've
already built (as will most of any other timing routines you build).

To make it more interesting:
  1. Trim as many nanoseconds off of the timings as possible and see that the 
     code below still works.
  2. Double-check the timings using your scope.

### Part 2: turn on one pixel (5 minutes)

As a "hello world" you should call your code to turn on one pixel to
a specific color.  Change the code in `simple.c` to turn on different
colors and different pixels.

### Part 3: build the neopixel inferface (20 minutes)

The files `neopixel.h` and `neopixel.c` define some trivial routines to add
buffering the a light string.  Finish implementing these (should be fast)
and verify the supplied cursor routine in `simple.c` does something.


### Part 4: do something cute with the interface.

A lot of people have gotten a lot of free trips and other things because
they were able to do tricks with light strips.   Try to come up with
something cute using you code.

#### Additional reading.


Some useful documents:
  * Adafruit [best practices](https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices)
  * [Protocol](https://developer.electricimp.com/resources/neopixels)
  * Nicely worked out [tolerances for the delays](https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/)
  * Nice photos and setup of a [level shifter](https://learn.adafruit.com/neopixel-levelshifter/shifting-levels)
  * Sparkfun cursory breakdown of the [protocol](https://learn.sparkfun.com/tutorials/ws2812-breakout-hookup-guide)
  * Discussion if you want to [drive many at once](https://learn.adafruit.com/adafruit-neopixel-uberguide/basic-connections)
