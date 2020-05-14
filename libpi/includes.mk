# setup useful variables that can be used by make.

ifndef CS240LX_2021_PATH
$(error CS240LX_2021_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif


LIBPI_PATH = $(CS240LX_2021_PATH)/libpi

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
AR = $(ARM)-ar
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
LPP = $(LIBPI_PATH)
LPI = $(LIBPI_PATH)/libpi.a
START = $(LPP)/start.o

MEMMAP=$(LPP)/memmap

INC = -I$(LIBPI_PATH)/include -I.  -I$(LIBPI_PATH)/

# -I$(LIBPI_PATH)/staff-src

CFLAGS = -Og -Wall -nostdlib -nostartfiles -ffreestanding -mcpu=arm1176jzf-s -mtune=arm1176jzf-s  -std=gnu99 $(INC) -ggdb -Wno-pointer-sign

ASFLAGS = --warn --fatal-warnings  -mcpu=arm1176jzf-s -march=armv6zk $(INC)

CPP_ASFLAGS =  -nostdlib -nostartfiles -ffreestanding   -Wa,--warn -Wa,--fatal-warnings -Wa,-mcpu=arm1176jzf-s -Wa,-march=armv6zk   $(INC)

ifdef USE_FP
    LIBM = $(LIBPI_PATH)/libm/libm-pi.a
    CFLAGS += -DRPI_FP_ENABLED  -mhard-float -mfpu=vfp -I$(LIBPI_PATH)/libm/include -I$(LIBPI_PATH)/libm
    CPP_ASFLAGS += -DRPI_FP_ENABLED  -mhard-float -mfpu=vfp
    LPI := $(LIBPI_PATH)/libpi-fp.a
endif

