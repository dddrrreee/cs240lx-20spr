## Fast networking using GPIO to build a 8n1 UART

Now that you can write and verify timing-accurate code, we are going
to use this to make a simple, fast pin-to-pin network on your pi and
see how efficient you can make it, both in reducing latency and in
increasing bandwidth.

Having a transmit and receive implementation will serve us well later,
when we want to connect sensors wirelessly, ship data to a server, etc.

Implementing a network and protocol between your pi's
is surprisingly simple.  You will:
  1. Connect the two pi's using two wires: one for transmit, one for
     receive.  Configure these as GPIO output and inputs respectively.
     The transmit pin from one pi should be hooked up to the receive
     pin of the other.

  2. Write an 8n1-UART implementation to send and receive messages between the 
     pi's (described more below).
     For background, [Wikipedia's 8n1-UART protocol writeup is 
     reasonable](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter).

  3. The main fragility of this approach is that since the GPIO pins
     have no buffering, if a message arrives when the recipient is not
     listening, the message will disappear.   As an extension: modify
     your pi so that when a message arrives, it receives an interrupt
     and can start processing immediately.

----------------------------------------------------------------
### Checkin

You need:
  1. Your own SW-UART implementation, with a dump from your logic
     analyzer showing that it satisfies the `8n1` timing specification
     for at least a 115200 baud rate.

  2. Also showing that a SW-uart ping-pong implementation can run 
     for thousands of iterations without errors.

  3. A parallel version of your SW-uart that can send some number
     (hopefully 8)
     bits in parallel and an adaptation of your analyzer to print out
     the traces from more than one pin.

There are many extensions (listed below).  Feel free to do some for the
shadow A+ grade!  A few key ones:

  1. Devise a custom network protocol that can send bits faster than ours. You should
     consider making it so that its easy to get one bit at a time in the interrupt
     handler.
  2. Tune the sw-uart so you can do at least 1Mb per second between two pi's for
     one pin.
  3. See how far you can overclock the pi by modifying `config.txt` on the 
     SD card to get even more performance :).
  4. Make the approach interrupt-based so we do not lose incoming characters.

----------------------------------------------------------------
### A quick and dirty guide to making 8n1 UART using GPIO

We sketch how to transmit and receive 8-bit bytes.

First, pick a baud rate `R` (bits per second) and compute the cycles per bit:

  1. Given baud rate `R`, compute how many micro-seconds `S` you write
     each bit.  For example, for 115,200, this is: `(1000*1000)/115200
     = 8.68us`.

  2. Since the pi can measure cycles more quickly and accurately than
     it can microseconds convert `S` to cycles per bit.  By default the A+
     runs at `700MHz` so that is `700 * 1000 * 1000` cycles per second
     or about `6076` cycles per bit.

Second, initialize the `TX` line at startup to be high (1).

Finally, given the cycles per bit `T` (from above: `T=6076` cycles),
transmit byte B as follows:

  1. Write a 0 (start) for `T` cycles.  (Why? Because it will always cause a transition given
     the TX line's default state of 1; this allows the receiver to detect when you 
     start sending.)
  2. Write the value of bit 0 for `T` cycles (0 if it is 0, 1 if it is 1).
  3. Write the value of bit 1 ...
  4. Write the value of bit 2 ...
  5. Write the value of bit 3 ...
  6. Write the value of bit 4 ...
  7. Write the value of bit 5 ...
  8. Write the value of bit 6 ...
  9. Write the value of bit 7 ...
  10. Write a 1 (stop) for at-least `T` cycles.

If you pause and look at the above list, you will notice it *is almost
completely identical to you test generator code from the last lab,*
with the main difference is that it determines whether to send a 0
or a 1 based on data rather than strictly alternating.  Fantastic!
You already know how to write and measure such code  with low error.
That ability will make this lab much easier.

(As an aside, the fact we can reuse seemingly disparate methods in
seemingly "new" domains is a good example of getting multiple things
from one action, which I always feel is the universe telling us we are
on a good path.)

Reception mirrors the above transmit steps:
 1. Wait for a start bit.   
 2. Delay for another `T+T/2` cycles so that we are in the center of the period 
    that the first data bit will be transmitted.
 4. Bit-wise or the value of the RX line into the bit at position 0.
 5. Delay another T cycles so we are in the center of the transmission of the next data bit.
 6. Bit-wise or the value of the RX line into the bit at position 1.
 7. ...
 8. Wait for the stop bit.    (Note: if you return immediately,
    without waiting for any stop bit, then you can get incorrect results:
    if the last data bit was 0 and you try to read another byte while
    it is still being transmitted you will think it is a start bit!
    Ask me how I know.)

Some notes:

   - If you look at this list, you'll realize that because of how the
     pi's GPIO was designed we can set and read the value of many pins
     with a single store or load.  As a result, reading and writing
     multiple pins costs no more than reading or writing a single pin.
     We can use this ability to speed up our protocol by sending and
     receiving as many bits in parallel as there are available pins.

   - As you notice from the protocol, it has no acknowledgements.
     As a result, while it is primarily a *serial* protocol, perhaps
     ironically it can be transparently used as a broadcast protocol: the
     sender does not care if there are one or many recipients because
     it does not have to name them, nor does it have to collect and act
     on O(n) acknowledgements.  I'm hopeful this will allow us to 
     do RF broadcasting without modification.

   - A limit of the protocol is that the nodes must rely on the baud rate
     *a priori*.   To be conservative, this baud rate will be slower
     than what they can handle.  As part of this lab you can take a
     stab at designing a protocol that will "self-clock" to the speed
     the recipient can keep up with.

   - As-is, the protocol does not work well for running in an interrupt
     handler where we pick off a bit per interrupt.    (E.g., if the
     last bits of the transmitted byte are 1, then you won't see
     any transitions if you are picking up rising/falling edges.)
     You could use an 8n2.  However, for expediency we typically run
     the entire sequence of steps to get a byte in the interrupt handler
     (yeah, gross).  This strategy means that the faster we can make the
     protocol, the less time you need to stay in the interrupt handler.

Many people are dismissive of a "bit-banging" approach to implementing
a hardware protocols (e.g., UART, I2C, SPI) rather than doing it like
a grown up and using hardware.  They will make vague statements that
bit-banging is slow, costly, error prone.  However, bit-banging has
several nice features, and in many cases they are completely wrong.

  1. It is simple.  You don't have to figure out datasheets and
     write a device driver for the peripheral you are trying to coax
     into doing the right thing: just blast out the 0s and 1s using
     GPIO pins.  As a benefit, you will get a much deeper understanding
     of whatever protocol you are building (e.g., compare your knowledge
     of UART after you finish this lab versus writing a hardware device
     driver for the Broadcom UART).

  2. It can avoid hardware error and limits.  E.g., the PCM output of
     the pi is known to be awful.  We can simply build our own cleaner
     ones.  The I2C implementation has low-power issues: same.

  3. It may be faster!  The pi actually has a pretty beefy CPU
     compared to the processors running in the peripherals.  You may be
     able to run at much higher speeds than they can.  You can certainly
     operate in parallel in ways that they cannot.  It's worth comparing
     your final baud and bandwidth to the maximum you can specify for
     the pi's hardware UART(s).

  4. As a variant of faster: bit banging can mean *we do not have to
     use memory barriers*.  Recall from the Broadcom document: every time
     we read or write different peripherals, we have to do a memory
     barrier to make sure that all previous operations retire before
     continuing, but we do not need to do memory barriers when using
     the same peripheral.  Thus, if we bit bang everything on GPIO and
     never switch, we can completely elide barriers.  Given their cost
     (measure them!) this is fantastic.

----------------------------------------------------------------
## Part 0: Test your hardware.

To make sure everything is working, first hook up your pi's and make sure
they can see each other transitions using a trivial protocol.  Make a copy
of your `test-gen` and change it so:

   1. Each pi runs in a loop for 4096 iterations waiting  on its `RX` pin: when `RX` changes, the pi
      sets its `TX` pin to the opposite value.
   2. Pick a pi to start the loop based on who reads their `RX` line as 0 first.  (This is 
      a race condition, but given human speeds, shouldn't happen in practice ---
      you can also make separate client and server binaries).
   3. Measure how fast this can go.  This is the upper bound on transition speed.
      I'm curious what it is!

----------------------------------------------------------------
### Part 1: write a software-UART implementation.

Implement two routines as described above for a baud rate of 115200:

    // transmit <b> 
    int sw_uart_put8(my_sw_uart_t *uart, uint8_t b);

    // blocks until it receives a character, returns < 0 on error.
    int sw_uart_get8(my_sw_uart_t *uart);

Notes:

  0. You can see `cs140-src/sw-uart.h` for an example structure definition.  Note
     you cannot divide on our pi setup, so you'll have to make sure to do division
     by a constant so that compiler can replace it.
  1. Check `sw_uart_put8` using your logic analyzer and make sure it
     has good, clean timings.
  2. You cannot directly check receive, since your analyzer cannot see
     pin reads.  As a hack, modify the code so that each time it reads
     the RX pin, it also sets a different pin high --- your scope can
     check that.
  3. Modify the code so that it sends an integer back and forth as a
     pin pong and that the value increases from 0 to 4098 with no errors.
     Measure how long this takes!

NOTE:
  - When you are debugging, if you put in a `printk` on the receiver, you
    can miss incoming bits.  Only print (or do things that are expensive)
    when you are not expecting input.  GPIO pins do not buffer input.
    This is a common mistake: I said this many many times in cs140e (and I've
    already said it twice in this document!), but
    at least 1/3 of all students ran into this issue at different points.

Great!  That is the main big conceptual hurdle.  The rest of the time you'll
just be making your code better.

----------------------------------------------------------------
### Part 2: increase baud rate.

As mentioned above, your UART implementation will be useful later.
So it
is worth making it fast --- the faster it is, the more time there is to
compute other stuff on the pi.

Using the techniques from the last lab, see how high you can make the
baud rate.  I have no idea what the limit is.  We seem to only be able
to get about 1Mb a second between the pi and a Unix system --- it would
be interesting if you could beat this.

Checkoff (from above):
  1. Your ping-pong still works, and it gets faster as we expect.
  2. Your scope timing diagram makes sense.

----------------------------------------------------------------
### Part 3: increase bandwidth.

As mentioned above, we can set and read many pins in parallel:

  1. Make a version of your code that will transmit and receive up to 8
     bits in parallel.  I would start with just two bits and make sure it
     makes sense.
  2. Extend your scope so that it works with multiple bits.

Checkoff (from above):
  1. Your ping-pong still works, and it gets faster as we expect.
  2. Your scope timing diagram makes sense.

----------------------------------------------------------------
### Extension: Speed: custom protocol.

Two downsides of the UART protocol:
  1. The baud rate is hard-wired in.  You may be able to go (much) fast and at other 
     times, may run much slower.
  2. It also can transmit when a receiver is not ready and drop bits.

Design a custom network protocol that solves these problems.
You should be able to send bits faster than ours and only transmit when 
the receiver is waiting.

As one possible approach: use a second line the receiver can use to signal
for when it is ready for the next bit: the sender can then adaptively
run as fast as possible.

I'm very curious how fast you can make this!

----------------------------------------------------------------
### Extension: Speed: over-clocking the pi to reduce error.

One way to get even more speed is to change the speed of the pi CPU, GPU,
memory and other things by modifying `config.txt` on the pi's SD card.
See how far you can reduce the error by changing these.  You'll most
likely have to resort to blog posts to get the full working range of
some of these parameters.

Some writeups:
  - [Official pi page](https://www.raspberrypi.org/documentation/configuration/config-txt/overclocking.md)
  - [Higher values](https://core-electronics.com.au/tutorials/overclocking-your-raspberry-pi.html)

I'm curious how fast you can get it and still have things work!   

Note, that if you change the GPU speed, you may not be able to bootload
b/c our UART assumes a fixed 250Mhz clock for it when it computes baud
rate.  You will likely have to change your hardware UART implementation.
I would save any GPU modifications for last.
