TARGET = libpi-fake.a
LPI = $(CS140E_PATH)/libpi/

# we could use time directly: you will have to build a PUT32 that returns something sensible.
#       SOURCES := $(wildcard ./*.c) $(LPI)/cs140e-src/timer.c
SOURCES := $(wildcard ./*.c) 
OBJS := $(patsubst %.c, %.o, $(SOURCES))

# add any other object files here.
# OBJS = fake-put-get.o pi-test.o pi-random.o fake-pi.o fake-gpio.o driver.o fake-time.o

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

ifndef CS240LX_2021_PATH
$(error CS240LX_2021_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

# where libpi is: probably should move most of this stuff into this directory?
LPI = $(CS140E_PATH)/libpi
# where libunix is (currently)
LUNIX = $(CS240LX_2021_PATH)/libunix/

CC=gcc
CFLAGS = -Wall -g -Og -std=gnu99  -Werror -Wno-unused-function 
CFLAGS += -I$(LPI) -I$(LUNIX) -DRPI_UNIX

# overdepend: all .o's depend on all .h's  --- make deps as simple as possible.
DEPS := $(wildcard ./*.h) $(LPI)/rpi.h Makefile

all: $(TARGET)

$(OBJS): $(DEPS) 

$(TARGET): $(OBJS) $(DEPS) 
	ar cr $(TARGET) $(OBJS) 

clean:
	rm -f *.o $(TARGET) *~  *.bak  *.out

.PHONY: all clean
