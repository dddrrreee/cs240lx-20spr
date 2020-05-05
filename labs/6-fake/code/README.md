### Part 1: Implement the code in `gpio-pud.c`

You can run `on.bin` to turn on the pullup/pulldown and `off.bin` to reset the pins to their
default values.


### Part 2: Make a fake libpi.

If you do a "make fake-libpi" it will compile fake versions.  Initially you 
will have a bunch of undefined references.
  1. Using the output in `on.out` and `off.out` make fake versions that print the 
     same thing.

  2. When you compile and run, you should get the same values.

  3. Then, modify the libpi-fake makefile to use the raw libpi `timer.c` code (uncomment
     the variable that references it) and modify your `get32` implementation to return
     a time value.   Make sure you get the same result as everyone else.

Bigger picture: There are three levels of fake:
  1. `off.out`: you simply provide stubs for all the routines
     (`gpio_set_pulldown`, `gpio_set_input`, `gpio_set_pullup`, and
     `gpio_pud_off`).  The output file gives the format we expect.
     (Everyone needs the same format so that we can do a simple string
     comparison of our results.)
  2. `on.timer.out`: you link in `cs140e-timer.c` into your fake library.
  3. `on.gpio+timer.out`: you also link in `gpio.c` into your fake library.

You should get the same output for all three.  The first is the easiest
to debug but tests the least.  The last is fairly harsh, but trickier
to reason about when it goes wrong.
