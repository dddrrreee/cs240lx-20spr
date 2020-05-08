LINK_OBJS := i2c.o
OBJS := accel.o gryo.o $(LINK_OBJS)

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_PATH)/libpi/includes.mk

CFLAGS += -Wno-unused-function -Wno-unused-variable -falign-functions=4


# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(wildcard ./*.h) ./Makefile 

# uncomment the different tests.
all: libpi $(OBJS) gyro.bin accel.bin run

libpi: 
	@ # @make -s -C $(LPP)
	make -C $(LPP)

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: %.o $(MEMMAP) $(LINK_OBJS) $(DEPS) 
	$(LD) $(START) $*.o $(LINK_OBJS) -T $(LPP)/memmap -o $*.elf  $(LPI)
%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

run:
	my-install ./accel.bin

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]

clean:
	rm -rf $(BUILD_DIR) gyro.bin accel.bin *~ tags gyro.o accel.o *.bin *.elf *.list *.img Makefile.bak  *.d

.PHONY: libpi all tags clean run

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf %.o
