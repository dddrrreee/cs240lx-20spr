TARGET_HL = libpi-fake.hl.a
TARGET_LL = libpi-fake.ll.a
LPI = $(CS140E_PATH)/libpi/

# we could use time directly: you will have to build a PUT32 that returns something sensible.
# if you want to use raw rpi code, put it before our replace
LOW_LEVEL_PI_SOURCE = $(LPI)/cs140e-src/timer.c $(LPI)/cs140e-private/gpio.c $(LPI)/cs140e-private/gpio-pud.c 

HL_SOURCES :=  $(wildcard ./*.c) 
LL_SOURCES := $(LOW_LEVEL_PI_SOURCE) $(wildcard ./*.c) 

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

VPATH := .:$(LPI):$(LPI)/cs140e-private:$(LPI)/libc:$(LPI)/cs140e-src:$(LPI)/cs140e-src
START= cs140e-start

LL_OBJS := $(patsubst %.c, %.o, $(LL_SOURCES))
HL_OBJS := $(patsubst %.c, %.o, $(HL_SOURCES))

BUILD_DIR := ./objs

LL_OBJS := $(foreach o, $(LL_OBJS), $(BUILD_DIR)/$(notdir $o))
HL_OBJS := $(foreach o, $(HL_OBJS), $(BUILD_DIR)/$(notdir $o))

CC=gcc
CFLAGS = -Wall -g -Og -std=gnu99  -Werror -Wno-unused-function 
CFLAGS += -I$(LPI) -I$(LUNIX) -DRPI_UNIX

# overdepend: all .o's depend on all .h's  --- make deps as simple as possible.
DEPS := $(wildcard ./*.h) $(LPI)/rpi.h Makefile 

all: $(TARGET_HL) $(TARGET_LL)

# XXX: We seem to need this rule to stop compilation with an error if the .c file
# has one: otherwise make does not stop compilation when generating .d.
# If compilation goes correctly, is not triggered.
$(BUILD_DIR)/%.o : %.c
$(BUILD_DIR)/%.o : %.c $(BUILD_DIR)/%.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<


# Rules to automatically generate dependencies and put in build directory
# We want to recompile everything if the makefile changes.
$(BUILD_DIR)/%.d: %.c ./Makefile
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -MT $@ -MMD -MP -MF $@ $< -o $(BUILD_DIR)/$(notdir $*).o

$(BUILD_DIR)/%.d: %.S ./Makefile
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPP_ASFLAGS) -c -MT $@ -MMD -MP -MF $@ $< -o $(BUILD_DIR)/$(notdir $*).o

# -MF  write the generated dependency rule to a file
# -MG  assume missing headers will be generated and don't stop with an error
# -MM  generate dependency rule for prerequisite, skipping system headers
# -MP  add phony target for each header to prevent errors when header is missing
# -MT  add a target to the generated dependency

# I don't get why we need this.  check the mad-make guy.
#   http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
$(BUILD_DIR)/%.d: ;
.PRECIOUS: $(BUILD_DIR)/%.d

$(TARGET_LL): $(LL_OBJS) $(DEPS)  ./Makefile
	ar cr $(TARGET_LL) $(LL_OBJS) 
$(TARGET_HL): $(HL_OBJS) $(DEPS)  ./Makefile
	ar cr $(TARGET_HL) $(HL_OBJS) 

clean:
	rm -f *.o *.a *~  *.bak  *.out 
	rm -rf objs

.PHONY: all clean
