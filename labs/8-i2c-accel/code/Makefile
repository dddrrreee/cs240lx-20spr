NAME := accel
OBJS := $(NAME).o i2c.o

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_PATH)/libpi/includes.mk

CFLAGS += -Wno-unused-function -Wno-unused-variable -falign-functions=4



# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(wildcard ./*.h) ./Makefile 

# uncomment the different tests.
all: libpi $(OBJS) $(NAME).bin run.first

run.first:
	my-install /dev/ttyUSB0 ./$(NAME).bin
run.last:
	my-install /dev/ttyUSB1 ./$(NAME).bin
    
libpi: 
	@ # @make -s -C $(LPP)
	make -C $(LPP)

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: $(MEMMAP) $(OBJS) $(DEPS) 
	$(LD) $(START) $(OBJS) -T $(LPP)/memmap -o $*.elf  $(LPI)
%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

run:
	my-install /dev/ttyUSB0 ./$(NAME).bin

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags $(NAME).o *.bin *.elf *.list *.img Makefile.bak  *.d

.PHONY: libpi all tags clean run

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
