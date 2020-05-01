NAME=liblxpi.a

# check that your path is defined.
ifndef CS240LX_2021_PATH
$(error CS240LX_2021_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_PATH)/libpi/includes.mk

CFLAGS += -Wno-unused-function -Wno-unused-variable -falign-functions=4

# if you want to compile everything in a directory, do something like this.
SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c, %.o, $(SOURCES))

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(wildcard ./*.h) ./Makefile 

all: libpi $(OBJS) $(NAME)
    
libpi: 
	@ # @make -s -C $(LPP)
	make -C $(LPP)

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

$(NAME): $(OBJS) $(DEPS)
	arm-none-eabi-ar crf $(NAME) $(OBJS)
	$(OD) -D $@ > $@.list

tags: 
	ctags *.[chSs] 

clean:
	rm -rf *.a *.o *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d

.PHONY: libpi all tags clean 
