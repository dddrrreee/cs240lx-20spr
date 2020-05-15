# add each test here that you want to check.
TESTS = part1-test1

# easy way to test everything when you are done.
# TESTS := $(wildcard ./part*-test*.c)

TESTS := $(patsubst %.c, %, $(TESTS))  

SYS_OBJS = ckalloc.o
ALL_SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c, %.o, $(ALL_SOURCES)) 
TEST_BINARIES := $(patsubst %, %.bin, $(TESTS))  
TEST_RUN := $(patsubst %, %.run, $(TESTS))  
TEST_EMIT := $(patsubst %, %.emit, $(TESTS))  
TEST_CHECK := $(patsubst %, %.check, $(TESTS))  

# if you want to compile everything in a directory, do something like this.
# SOURCES := $(wildcard ./*.c)

# check that your path is defined.
ifndef CS240LX_2021_PATH
$(error CS240LX_2021_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS240LX_2021_PATH)/libpi/includes.mk

CFLAGS += -Wno-unused-function -Wno-unused-variable 

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(LIBM) $(wildcard ./*.h) ./Makefile 

# i *think* order is important.
LIBS = $(LPI)  

all:  libs $(OBJS) $(TEST_BINARIES) $(TEST_RUN) # $(TEST_CHECK)

libs: 
	make -C $(LPP) 

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: %.o $(OBJS) $(MEMMAP) $(DEPS) 
	$(LD) $(START) $*.o $(SYS_OBJS) -T $(LPP)/memmap -o $@  $(LIBS)

%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

%.run: %.bin
	my-install ./$*.bin

# note: this will fail if output does not contain a trace.
%.emit: %.bin
	my-install ./$*.bin 2>&1 > /dev/null | grep 'TRACE:\|ERROR:\|PANIC:' > ./$*.out

%.check: 
	my-install ./$*.bin 2>&1 > /dev/null | grep 'TRACE:\|ERROR:\|PANIC:' > ./$*.test
	diff ./$*.test ./$*.out

# not sure if there is a better way: use these fake target to run our
# checks for everything.
emitall: $(TEST_EMIT) $(TEST_BINARIES)
checkall: $(TEST_CHECK) $(TEST_BINARIES)
runall: $(TEST_RUN) 

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs] 

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags $(OBJS) *.bin *.elf *.list *.img Makefile.bak  *.d

cleanall:
	make -C ./ clean
	rm *.out

.PHONY: libs all tags clean run fake-libpi  runall emitall checkall

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
