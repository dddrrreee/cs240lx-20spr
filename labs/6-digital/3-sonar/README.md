### Measuring distance with a cheap sonar device
	
For this lab you're going to implement and a distance measuring device
that uses sonar using the HC-SR04 device.  At a high level it sends a
high frequency pulse (if you have a cat, it may get annoyed) and tells
you when/if it receives it back.  You use the time between pulse and
receive to compute distance using the speed of sound.

As with the previous lab, there's some simple skeleton code, but you're
going to do all the work yourself, including getting the data sheets
and anything else you need.  This process will foreshadow what you'll
have to do when you write drivers for devices on your own (with the
simplification that in these two cases we know its possible to get the
needed information!).

<table><tr><td>
  <img src="images/hc-sr04.jpg"/>
</td></tr></table>

### Check-off

For lab:
   1. Use a ruler (we'll have one) to check that your sonar measures distance accurately.
   2. Show that you can handle when your sonar does not hear an echo back.
   3. Compute checksums and compare to everyone else.

If you run `make test` the `Makefile` will compute the checksums for you:
    ./fake-sonar 1 2> /tmp/sonar-1.out
    cksum /tmp/sonar-1.out
    1950925016 7211 /tmp/sonar-1.out
    ./fake-sonar 10 2> /tmp/sonar-10.out
    cksum /tmp/sonar-10.out
    2186614110 142109 /tmp/sonar-10.out

Extension: 
   - Vary an LED smoothly using PWM based on the distance (e.g.,
    close is bright, far is less so).  This problem is trickier than
    it seems!

----------------------------------------------------------------------
### Implementing the HC-SR04 sonar driver.

First, get the datasheet.  The first one is a 3-page sheet that isn't
super helpful (common); there's a longer one if you keep digging.
Also, the sparkfun comments for the device have useful factoids (but
also wrong ones, which --- you guessed it --- is common).

Again, you're going to be skimming through the data sheet looking
for the information you need.

Second, implement three functions:
 1. `hc_sr04_init(trigger, echo)`: called with the GPIO pins to use for 
    `trigger` and `echo`.  It sets these up and initializes the device.
 2. `hc_sr04_get_distance`: compute the distance in inches using the sonar
     and returns it.
 3. `read_while_eq(pin,v,timeout)`, a helper function that blocks reading `pin`
     until either (1) `gpio_read(pin)` is not equal to `v` or (2) `timeout`
     microseconds have expired.   

     This is a handy function for this lab and later.  In general, when
     we do measurements in the real world the measurement can fail.
     For example, for today's lab, if the sonar pulse never echos back
     (because it got absorbed or because it hit a surface at a sharp
     enough  angle that it ricocheted off) then the naive code will just
     lock up.  `read_while_eq` handles this case by returning an error
     if it times out.

     You should look in `libpi/cs49n-src/timer.c` to see how to handle
     the case that time wraps around.

Some additional hints, you'll need to:

 0. Determine if any pin need to be set as `pulldown`.  We provide a
    `gpio_pulldown` but you should start replacing our code with your own.
    (using the broadcom pdf in the `docs/` directory).  If you don't 
    the device will often (maybe always) work, but could easily not.
 1. Initialize the device (pay attention to time delays here).
 2. Do a send (again, pay attention to any needed time delays).
 3. Measure how long it takes to receive the echo and compute round trip
    by converting that time to distance using the datasheet formula.
    Note, we do not have floating point!  So the way you compute this must
    use integer arithmetic.  Note, too, that we don't even have division!

Trouble-shooting.

  1. Readings can be noisy --- you may need to require multiple
  high (or low) readings before you decide to trust the signal.
  2. There are conflicting accounts of what value voltage you
  need for `Vcc`.
  3. The initial 3-page data sheet you'll find sucks; look for
  a longer one. 

----------------------------------------------------------------------
### Extension 2: use the Sonar distance to control an LED.

As your distance gets closer or farther,  vary the LED brightness **smoothly**.
I'd break it down:

   1. Write the PWM code --- empirically figure out what how coarse your
      delays can be to make the LED not flicker.
   2. Make sure you can change the led smoothly from off to full brightness.
   3. Hook it up to the Sonar: the trick is to make sure it doesn't make big jagged
      changes.

We'll use this ability for later labs.
