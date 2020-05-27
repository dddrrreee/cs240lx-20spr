### TSOP4838 IR sensor.


Today we're going to use the TSOP4838 IR sensor to reverse engineer the
key presses from a remote so that you can control your pi using one.
For some reason I got an inordinate amount of fun from doing this project
--- I think it's a good example of how with just a tiny bit of code
and even a simple sensor you can go from passively button pushing modern
devices to seeing how they work and re-purposing them for your own purposes.

Why:
  1. We haven't done enough devices; the TSOP is a useful one.
  2. It shows how to use time to communicate additional information digitally.
  3. It's often useful to add a remote to your projects.  After today's lab
     you'll be able to do so in a few minutes by just re-purposing your code.
  3. You can use it, the IR led that was mailed out earlier, and your UART code
     to make a bare-metal wireless network without any additional hardware.
     It won't be high-bandwidth, but it should be pretty simple,  (Certainly
     compared to the 74 page Nordic datasheet that we have for our RF transceiver!)

What you'll need:
  1. The TSOP4838 that you received in the mail.
  2. The data sheet in the `docs` directory.
  3. Any remote you have laying around you house.

If you do the wireless network:
  1. Two of the pale blue IR led's that you received and the second TSOP.
  2. Your second pi.
  3. Your UART code from lab 3.


### Checkoff:
  1. You should be able to print out which key your remote is sending.
  2. Ideally you can either (1) send and receive wirelessly or (2) use
     GPIO interrupts to grab a remote signal.

----------------------------------------------------------------
### How do remotes send values?

The remote I used appears to send 0 or 1 bit by turning its IR LED on
for differing amounts of time.   It separate the bits turning the LED off
for a fixed amount of time.  (You might read up on Manchester encoding.)
    - skip (separate bits): turn off for about 600usec.
    - 0: turn on for about 600usec.
    - 1: turn on for about 1600 usec.

So to send `01` they would:

    1. Send a skip (off for 600 usec).
    2. Send a 0 (on for 600 usec);
    3. Send a skip (off for 600 usec);
    4. Send a 1 (on for 1600usec).
    5. (maybe) send a skip (off for 600usec).

It also looks like they indicate the start of a transmission by sending
1 for about 40,000 usec and then an initial value.

-------------------------------------------------------------------
### Part 0: House-keeping:

You should copy code from a recent lab into this directory, update the
`Makefile` and look over the datasheet in the docs directory. The TSOP
has a pretty similar interface to a hall effect sensor.

First steps:
   1. Write the code to read from the input pin.  

   2. Remotes communicate by sending values for a specific amount of
      time.  So to start off record the value sent and the time it was sent.
      Print these records out when you timeout waiting for more transitions.

   3. COMMON MISTAKE: don't print *while* timing, since that messes up
	  the timing.  Instead when the IR has been low for "a long time",
	  emit all the timings you've seen.

----------------------------------------------------------------
#  Part 1: reverse engineering key presses.

Given the protocol described above, you should be able to reverse engineer the
value `v` for each key. 

   0. Set at 32-bit unsigned variable `v` to 0.
   1. Wait until you see a start bit (sort of similar to the UART protocol you built).
   2. For each on pulse, record its bit position, and convert it to a 0 or 1 
      (depending on its length, as above).
   3. For any 1 bit in (2) set the bit at that position in `v`.  
   4. When your code gets a timeout, print `v` at the end.
   5. If you hit the same remote key over and over, you should get the same value.
  
The main issue is handling error.  I did something pretty simple: 
   1. Pick some acceptable amount of error (in my case, 200 microseconds).
   2. If the time is within `eps` of the value for 1, return 1.
   3. ...  return 0.
   4. Otherwise `panic.

Feel free to be more clever!

    enum { ir_eps = 200 };

    static int abs(int x) { return x < 0 ? -x : x; }

    static int within(unsigned t, unsigned bound, unsigned eps) {
        return abs(t-bound) < eps;
    }

    // return 0 if e is within eps of lb, 1 if its within eps of ub
    static int pick(struct readings *e, unsigned lb, unsigned ub) {
        assert(lb<ub);
        if(within(e->usec, lb, ir_eps))
            return 0;
        if(within(e->usec, ub, ir_eps))
            return 1;
        panic("invalid time: <%d> expected %d or %d\n", e->usec, lb, ub);
    }


NOTES:
   1. Use a `gpio_pullup` to pull the input pin high when nothing is happening.
   2. As we found in the early timing labs, if you print something while you
      are waiting for a time-based signal, the print will likely take long enough
      that you miss it or corrupt the timing.
   3. As mentioned before, since the timer on the pi is a different device than 
      GPIO, you'd have to use a memory barrier to synchronize them.  To avoid this,
      you might want to use the pi's cycle counter (which doesn't need a memory
      barrier).  Of course, this can overflow pretty quickly, so make sure your
      code does not get confused.


----------------------------------------------------------------
#  Part 2: Using key presses.

After you get all the values for the key presses, the easiest approach
is to make a set of `enum` values to encode them and make a routine that
will return which one of these it is.

You should wrap this up and show that you print each key correctly.

----------------------------------------------------------------
#  Part 3: interrupts.

In general, if we use remotes its to very-occasionally control a device
that is pretty busy doing something else.  In this case, it's natural
to use interrupts (note: this is hard if we care about very fine timing;
there are other approaches).

If you need a refresher:
  - lab 8 in cs140e goes over GPIO interrupts.
  - I checked in a staff version `staff-objs/gpio-int.o` that you can link in
    (by modifying `put-your-src-here.mk`) and use.
  - If you have it, you can also use your own code.


The relevant functions:

    // include/gpio.h
    
    // p97 set to detect rising edge (0->1) on <pin>.
    // as the broadcom doc states, it  detects by sampling based on the clock.
    // it looks for "011" (low, hi, hi) to suppress noise.  i.e., its triggered only
    // *after* a 1 reading has been sampled twice, so there will be delay.
    // if you want lower latency, you should us async rising edge (p99)
    void gpio_int_rising_edge(unsigned pin);
    
    // p98: detect falling edge (1->0).  sampled using the system clock.  
    // similarly to rising edge detection, it suppresses noise by looking for
    // "100" --- i.e., is triggered after two readings of "0" and so the 
    // interrupt is delayed two clock cycles.   if you want  lower latency,
    // you should use async falling edge. (p99)
    void gpio_int_falling_edge(unsigned pin);
    
    // p96: a 1<<pin is set in EVENT_DETECT if <pin> triggered an interrupt.
    // if you configure multiple events to lead to interrupts, you will have to 
    // read the pin to determine which caused it.
    int gpio_event_detected(unsigned pin);
    
    // p96: have to write a 1 to the pin to clear the event.
    void gpio_event_clear(unsigned pin);

Simple strategy:
   1. enable rising edge (from 0 to 1).
   2. In the interrupt handler, just process the entire remote transmission.
   3. This locks up the pi during this, but in the case that you are going to
      do a major phase shift, this is ok.

A possibly better strategy:
   1. Detect each edge and put the result in a queue.
   2. When you detect a stop, convert the edges to a value.
   3. This is more complicated, but lets you spend not-much time in the handler.

----------------------------------------------------------------
###  Part 4: simple networking

For this lab you're going to use a TSOP3223 infrared (IR) receiver and
a IR led to communicate with your second pi.

#### Simple transmit for LED

Using the IR led (blue, translucent) you will send a signal to communicate
with the TSOP.
 - You don't just turn the LED on and off.  The TSOP looks for a specific signal
   of off-on sent at a specific Khz and rejects everything else.
 - So look in the datasheet for the frequency it expects.
 - Compute the microseconds for on-off.

Compute it as:

    usec_period = 1./freq * 1000. * 1000.
    usec on-off = usec_period / 2.

Alternate sending this signal and show you can blink your second pi's LED.

#### Simple transmit for UART

Re-purpose your UART protocol so that you can send arbitrary bytes using
the TSOP.  Show you can print `hello world!`   Have the two pi's send a
counter back and forth similar to lab 3.
