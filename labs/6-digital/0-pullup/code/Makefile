

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_PATH)/libpi/includes.mk

CFLAGS += -Wno-unused-function -Wno-unused-variable -falign-functions=4

# if you want to compile everything in a directory, do something like this.
SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c, %.o, $(SOURCES))

LINK_OBJS := $(OBJS)
LINK_OBJS := $(filter-out ./on.o,$(LINK_OBJS))
LINK_OBJS := $(filter-out ./off.o,$(LINK_OBJS))

# OBJS := $(NAME).o

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(wildcard ./*.h) ./Makefile 

LXP = $(CS240LX_2021_PATH)/liblxpi
LXPI = $(LXP)/liblxpi.a

# i *think* order is important.
LIBS = $(LXPI) $(LPI)  

# uncomment the different tests.
all: libs $(OBJS) on.bin off.bin # fake-libpi
    
fake-libpi:
	make -f Makefile.fake

libs: 
	@ # @make -s -C $(LPP)
	make -C $(LPP)
	make -C $(LXP)

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: %.o $(OBJS) $(MEMMAP) $(DEPS) 
	$(LD) $(START) $< $(LINK_OBJS) -T $(LPP)/memmap -o $@  $(LIBS)

%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

run:
	my-install ./on.bin

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d

.PHONY: libs all tags clean run fake-libpi

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
