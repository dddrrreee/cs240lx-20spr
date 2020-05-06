### Measuring distance with a cheap sonar device and cross-checking results.
	
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
   4. Start doing one of the extensions so we have an idea of what's best to do for
      next lab.

----------------------------------------------------------------------
### Part 1: Implementing the HC-SR04 sonar driver.

First, get the datasheet.  The first one is a 3-page sheet that isn't
super helpful (common); there's a longer one if you keep digging.
Also, the sparkfun comments for the device have useful factoids (but
also wrong ones, which --- you guessed it --- is common).

*Note: to save more time for an extension, I checked in a bunch of
datasheets in the `docs/` directory in this lab.***

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

     You should look in `libpi/cs140e-src/timer.c` to see how to handle
     the case that time wraps around.

Some additional hints, you'll need to:

 0. Determine if any pin need to be set as `pulldown`.  If you don't 
    the device will often (maybe always) work, but could easily not.
 1. Initialize the device (pay attention to settling time delays here!).
 2. Do a send (again, pay attention to any needed time delays).
 3. Measure how long it takes to receive the echo and compute round trip
    by converting that time to distance (in inches, very crude) using
    the datasheet formula.  Note, we do not have floating point!  So the
    way you compute this must use integer arithmetic.  Note, too, that
    we don't even have division!

Trouble-shooting.

  0. Sonar uses reflection: if the device is too close to a surface on any side (typically
     its bottom) it can get garbage readings.  If you use a breadboard put the sensor so the emitters
     (the two silver cones) are as close to the edge as you can make them.
  1. Readings can be noisy --- in the real world you may need to require multiple
     high (or low) readings before you decide to trust the signal.  We ignore this problem today.
  2. There are conflicting accounts of what value voltage you need for `Vcc`.
  3. The initial 3-page data sheet you'll find sucks; look for a longer one. 

Expected tests and their results:
  1. If you aim the device where it can't get a reflection, it should return `-1` --- in 
     particular do not deadlock!
  2. If you use a ruler and a hard surface (the meat of your hand is probably not sufficiently
     reflective) you should get reasonable readings.
  3. If you come up with addition tests, please suggest them.

----------------------------------------------------------------------
### Part 2: Extending your fake-pi


#### High level fake pi.

Your libpi should work fine "as is."  Before going further: verify that
you get the same checksums as everyone else.  

Do the high-level version first.  Check this by:
   1. Commenting out `timer_get_usec` in `sonar.c` (you will put this back later).
   2. running `make` You may need to implement some undefined references.
   3. Synonyms: some people call `gpio_set_on(pin)` others call
      `gpio_write(pin,1)` --- both will do the same thing.   Just make `gpio_set_on` to
       call `gpio_write` instead.

My checksum for the high-level version is:

    % ./sonar.fake | cksum
    2561594719 9552

First 20 lines is:

    TRACE:fake_random:random called=1 times, value=1804289383
    TRACE:uart_init:uart
    PI:starting sonar!
    TRACE:gpio_set_output:pin=20
    TRACE:gpio_write:pin=20, val=0
    TRACE:gpio_set_input:pin=21
    TRACE:gpio_set_pulldown:pin=21
    PI:sonar ready!
    TRACE:gpio_write:pin=20, val=1
    TRACE:delay_us:delay_us = 10usec
    TRACE:gpio_write:pin=20, val=0
    TRACE:delay_us:delay_us = 148usec
    TRACE:timer_get_usec:getting usec = 1804289542usec
    TRACE:fake_random:random called=2 times, value=846930886
    TRACE:gpio_read:pin=21, returning=0
    TRACE:timer_get_usec:getting usec = 1804289543usec
    TRACE:fake_random:random called=3 times, value=1681692777
    TRACE:gpio_read:pin=21, returning=1
    TRACE:timer_get_usec:getting usec = 1804289544usec
    TRACE:timer_get_usec:getting usec = 1804289545usec

Of course, each time you run, you will get the same value --- as you
recall, you made your fake time value increment by 1 each time the timer
call is invoked.  Thus, we'll quit as soon as it gets the same value.
This isn't that interesting.

You can see this if you grep for "distance" you'll see  we never do anything as is:

    % ./sonar.fake  | grep dist
    PI:distance = 0 inches
    PI:distance = 0 inches
    PI:distance = 0 inches
    PI:distance = 0 inches
    PI:distance = 0 inches
    PI:distance = 0 inches
    PI:distance = 0 inches
    PI:distance = 0 inches
    PI:distance = 0 inches
    PI:distance = 0 inches


When we fake something that is important, we often have multiple
choices.  And the choice that is "best" depends on what we are testing.
One approach is to try to build every possible option into the tool.
This is a huge amount of work and will typically fail anyway, since most
people can't anticipate all possible situations.

As a first cut,  we'll exploit the linker and add a simple routine in
the driver.  Uncomment out `timer_get_usec` in `sonar.c` --- you'll
notice it's s protected by the preprocessor flags `RPI_UNIX` and
`FAKE_HIGH_LEVEL` only defined during fake compilation for high-level:

    #ifdef RPI_UNIX
    #   include "fake-pi.h"
    
    #   ifdef FAKE_LOW_LEVEL

        ...

    #   elif defined (FAKE_HIGH_LEVEL)

        unsigned timer_get_usec(void) {
            unsigned t = fake_time_inc( fake_random() % (timeout * 2) );
            trace("getting usec = %dusec\n", t);
            return t;
        }

    #   else
    
    #       error "Impossible: must define FAKE_HIGH_LEVEL or FAKE_LOW_LEVEL"
    
    #   endif
    
    #endif

You now have a definition of `timer_get_usec` customized to our domain
--- we want to timeout some number of times to test that code, and also
cover a wider range of distances than 0.

After this is set, we get:

    % ./sonar.hl.fake | cksum
    196199192 21103


My first 20 lines:

    TRACE:fake_random:random called=1 times, value=1804289383
    TRACE:uart_init:uart
    PI:starting sonar!
    TRACE:gpio_set_output:pin=20
    TRACE:gpio_write:pin=20, val=0
    TRACE:gpio_set_input:pin=21
    TRACE:gpio_set_pulldown:pin=21
    PI:sonar ready!
    TRACE:gpio_write:pin=20, val=1
    TRACE:delay_us:delay_us = 10usec
    TRACE:gpio_write:pin=20, val=0
    TRACE:delay_us:delay_us = 148usec
    TRACE:fake_random:random called=2 times, value=846930886
    TRACE:timer_get_usec:getting usec = 1804330427usec
    TRACE:fake_random:random called=3 times, value=1681692777
    TRACE:gpio_read:pin=21, returning=1
    TRACE:fake_random:random called=4 times, value=1714636915
    TRACE:timer_get_usec:getting usec = 1804397342usec
    TRACE:fake_random:random called=5 times, value=1957747793
    TRACE:timer_get_usec:getting usec = 1804475135usec


#### Low-level fake-pi

For the low-level, we can be a bit fancier.

Instead we use an under-appreciated method to make sure any possible
decision can be made:
  1. Give clients a way to plug in code into the system so that the checking system
     can call the client code when it needs to make a decision.
  2. If the client code is written in a Turing complete language, then it
     can implement any computable policy: no anticipation is needed.

Great, but this is too vague: plug in what?  And where?  In the case of
`lipi-fake` and many other checking systems, it turns out  (IMO at least)
that you can typically go very far, with very low effort by allowing
clients to override the "memory model."  I.e., what happens on a load
or store, or  --- in our case --- what the effect of doing a `PUT32`
is and what value `GET32` returns.

As you probably started to realize, we already played this game, in a
simplistic way last lab: we had some semantics for when you read from the
timer address (return a monotonically increasing counter) versus all others
(return a random value).  And, as the last lab notes, you can imagine
all sorts of memory models:
   1. Return the value of the last write (sequential consistency).
   2. Return a random value (which we did).
   3. Return an previously observed value from an actual run (so that it
      is a result that could be observed).

The reason this problem comes up today is that, as mentioned above,
we keep our old hard-wired memory model, your sonar code will only ever
output the shortest distance it can measure since the time increases by
one on each invocation.

For sonar, we'd like something that returns a random value in a range so
we can do multiple runs that try different values.  (Alternatively, we
could call `fork()` and test each possibility time returned!  We defer
this systematic til later.)  Easy enough.  Why not just build this
decision into `libpi-fake` and be done with it?  Well, it wouldn't work
well for `gpio_set_pulldown` and `gpio_set_pullup`.  The fact that one 
decision for one program sucks for another, and the reasons are program-specific 
is a hint that you should allow parameterization.

So today, you will implement two simple methods that `libpi-fake` calls:

    // returns 1 if it decided on a value for <val>.  otherwise 0.
    int mem_model_get32(volatile void *addr, uint32_t *val);

    // performs any side-effect on <addr> using val, returns 0 if it
    // did nothing.
    int mem_model_put32(volatile void *addr, uint32_t val);


You should:
  1. Put in default implementations into two new files in the `libpi-fake` directory:
     the default `mem_model_get32` always returns 0, the default `mem_model_put32` does
     nothing.  
  2. You should alter your `PUT32` and `GET32` implementations in `libpi-fake` to call
     memory model routines appropriately: if they return 1, defer, otherwise act
     as before.
  3. In the sonar directory: add a `mem_model_get32` that returns a
     random value (using `fake_random()`).  in the range of `0` to some
     max value (you should agree with everyone else what makes sense)
  4. Finally: modify your fake-pi driver so that it optionally takes a
     count of the number of trials to run and forks each execution,
     seeding the random to the loop counter.  

Make sure you get the same results as everyone else!

    Footnote: For related reasons, that you may want to think about,
    the fact that we can check so much of the code by simply monitoring
    `PUT32` and `GET32` means we can also control it thoroughly by
    overriding these.  I might go so far as to assert that there is
    likely a theorem here of some kind.


To run the low level:

  1. Change the Makefile so you use `FAKE_LOW_LEVEL`:

        LEVEL := FAKE_LOW_LEVEL
        # LEVEL := FAKE_HIGH_LEVEL


  2.  For the low level I got:

        % ./sonar.fake | wc
        1309    3110   63119
        % ./sonar.fake | cksum
        748725060 63119

----------------------------------------------------------------------
### Extension 1: use two sonar's to more accurately estimate position.

I have not done this. It could be nonsense.

However, *in theory* you can use the readings from two sonar to accurately
estimate position (just as you do with your eyes).  If you track position
over time, you can get velocity, and predicted movement.  Try doing this!
I'm very curious how accurate this can be made.

----------------------------------------------------------------------
### Extension 2:  hook your sonar up to your ws2812b light strip.

Often we want to use one sensor to control the other.  A pretty common
blinky-type approach is take the readings and map them to a light string
(which, you have!).

Suggested:
  1. Use change in distance to influence color (e.g., fast = hot, red, slow = cool, blue).
  2. Display the distance of both from both --- make the left red, the right blue, and overlay
     so you can easily see the delta.

----------------------------------------------------------------------
### Extension 3: adapt your logic analyzer to trace and replay signals.

One nice feature of our logic analyzer is that b/c we wrote it in a Turing
complete language (rather than bought a black box off of amazon or using a
fixed FPGA design) we can easily adapt it in all sorts of computable ways.

A cute, easy trick: instead of merely recording and reporting on the
signals, use the record to replay the result back.    For this extension,
change your logic analyzer so that it can:
  1. Record the signals for some amount of time from your pi to the sonar system.
  2. Replay these back to the pi system later and
  3. Flag if any output is different.

Making an electrical record-and-replay system has some great uses:

   1. While our `fake-pi` has some really nice features, it cannot guarantee how
      your code runs on real hardware.  By using electrical replay, you can
      test *for sure* how your code behaves.  Virtual is good.  Real is good.
      Being able to do both is great.

      (We ignore now but discuss later: the signals will have slightly different
      timings under replay, we may have to specify multiple legal outcomes.)

   2. No-effort automatic testing: testing embedded code that
      interacts with the physical world can be labor intensive, since
      it can require you modify the physical environment to run each 
      individual test.

      For example, for sonar: to test that your code works with an
      object 10 inches away, you have to put the object there (exactly).
      Then run the code, then check the output.  Two feet?  Move it
      again and rerun and retest.  Some other distance?  You guessed
      it. No signal?  Set it up so the signal does not come back, etc.
      Each test takes time and effort.  It's unlikely that after each
      sonar code modification you will run all your old tests. As a result,
      over time, it's likely you code becomes broken and, to add insult
      to injury, in a way that would be easy to detect.

      Replay removes this problem: record the signals on a real setup,
      and then just replay to the pi using the electrical test-generator,
      checking its output (`printk`, GPIO pins, etc) without requiring
      any physical modification.  Same input giving same output is a
      passed test.  Fantastically: you can also ship the test to other
      people in the class, and they never have to physically run it in
      the first place!  This lets you amplify the result of one action.
      (And also cross-check your results.)

      In cases where different runs should be independent, this approach
      lets you explode a past single run into many: combine different
      single traces into larger composed traces and make sure the
      output is equivalent.  (As a fun hack, you can get fancy and
      get exponential speedups by terminating compositions when the
      code gets back into a memory state that you have seen before
      --- you can detect this by hashing the stack, heap and data.
      State hashing is a cool hack used in expensive verification tools
      such as model checkers and symbolic execution systems and even
      some static analyzers.)

   3. Determinism: embedded code generally aims to be deterministic:
      same input + same timings = same result.   Testing determinism is
      a special case of the previous point and it's hard for the same
      reason, plus, it can be difficult to do physical interactions
      "exactly the same" from run to run.  For example, checking
      behavior after a button press --- did you do it at exactly the same
      nanosecond delay?  For sonar: is your measured object *exactly*
      in the same place it was last time?

      Replay removes this problem: record the signals on a real setup,
      and then just replay them over and over and over.  The result
      should always remain the same.

      Non-determinism may sound vague and uninteresting.  But it
      burned several people this quarter and many in previous ones.
      One example that showed up a couple of weeks ago: when you check
      for the value of a GPIO input pin, if you do not mask off of the
      other bits in the GPIO level register, you can falsely think the
      pin was non-zero if some other pin was set to 1.    Multiple replay
      tests could expose this issue.

      A second, more common example: if your program has a memory
      corruption bug, it will often not behave the same across multiple
      versions.  As you change the code, sometimes the corruption will
      hit one variable, sometimes another, causing different output
      modifications.   Determinism checks will catch this (especially
      if you do auxiliary tricks such as heap randomization or thread
      schedule modifications).

   3. Test amplification through distortion: given a trace `T`, there
      are often rules that allow you to (1) distort it into many other
      traces `T1`, `T2`, ..., `Tn` where (2) you know the expected answer
      of the distortion.

      For example: given a sonar trace we know we can simply not send
      back an echo and the pi sonar system should timeout and retry.
      Or, we know we can double the time of the echo and the output
      number should double as well.

      Note: as you probably guessed, as trace mutations get fancier, their
      knowledge of the checked protocol typically goes up.  Generally,
      we know that simply deleting responses should cause a timeout "of
      some kind", no matter the device.  However, modifying an input and
      predicting the scaler effect on an output requires more semantics.

    4. Some others.  It's getting long so I'll stop.
