# we could use time directly: you will have to build a PUT32 that returns something sensible.
# if you want to use raw rpi code, put it before our replace
LOW_LEVEL_PI_SOURCE = $(LPI)/cs140e-src/timer.c $(LPI)/cs140e-private/gpio.c $(LPI)/cs140e-private/gpio-pud.c
