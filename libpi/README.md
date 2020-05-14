Main rule:
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!



Organization:
  1. All the pi specific .h's are in `./include`.
  2. All the staff provided source is in `staff-src/`.   You can write your own, but 
     these are not super interesting.
  3. `./libc`: `.c` implementation of standard library functions (often ripped off from
    `uClibc`).
  4. `./libm`: `.c` implementation of standard math library functions (ripped off
     from BSD's libm).
  5. `staff-objs`: these are `.o` files we provide. You can always write your own
     and swap them by modifying `./put-your-src-here.mk`.

  6. `objs` and `objs-fp`: this is where all the .o's get put during make.
     you can ignore it.

Simple rules:
  1. Don't add files to `staff-src/` or modify our `.h` files.
     Files we add later may conflict with yours.

  2. If you want to add extra prototypes there is a `libc-extra.h` and an `rpi-extra.h`
     which we will not touch, so you can change freely.  

Basic idea: to add code:
  1.  Put your `.c` or `.S` file in `./src/`.
  2.  Add the path to the source in `put-your-src-here.mk` so that the
      the `Makefile` will compile them.
  3. Never modify the `Makefile` directory!


