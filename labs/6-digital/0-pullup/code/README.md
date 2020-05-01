Implement the code in `gpio-pud.c`

You can run `on.bin` to turn on the pullup/pulldown and `off.bin` to reset the pins to their
default values.

If you do a "make fake-libpi" it will compile fake versions.  Initially you 
will have a bunch of undefined references.
  1. Using the output in `on.out` and `off.out` make fake versions that print the 
     same thing.

  2. When you compile and run, you should get the same values.

  3. Then, modify the libpi-fake makefile to use the raw libpi `timer.c` code (uncomment
     the variable that references it) and modify your `get32` implementation to return
     a time value.   Make sure you get the same result as everyone else.
