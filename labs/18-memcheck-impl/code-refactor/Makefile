# if you do:
#   - "make checkall" it will check all programs in TESTS against their .out
#   - "make emitall" it emit .outs for all programs in TESTS
#   - "make runall" it will run all programs in TESTS
# if you want to just grab all tests do:
TESTS = ./tests/part2-test1.c 
TESTS = ./tests/part2-test2.c 
TESTS = ./tests/part2-test3.c
TESTS = ./tests/part2-test4.c
TESTS = $(wildcard ./tests/part*-test*.c )
# TESTS = ./tests/test7.c
# TESTS = ./tests/part1-test1.c 
# TESTS = ./tests/17-part1-test2.c
# TESTS := $(wildcard ./tests/16-part2-test*.c)
# TESTS := $(wildcard ./tests/test*.c)

# part1-test2 part1-test3 part1-test4
# part1-test1 part1-test2 part1-test3 part1-test4

# add each test here that you want to check.
# TESTS = c # part1-test1 

# easy way to test everything when you are done.
#
# all the part1 tests.
# TESTS := $(wildcard ./part1*-test*.c)
# all the part2 tests

TESTS := $(patsubst %.c, %, $(TESTS))  
GREP_STR := 'TRACE:\|ERROR:\|PANIC:' 

# ALL_ASM := $(wildcard ./*.S)
# $(patsubst %.S, %.o, $(ALL_ASM))  

# system-call.o

SYS_OBJS = interrupts-asm.o mmu.o mmu-helpers.o memcheck.o cp14-debug.o user-mode-run-fn.o  syscall.o memcheck-asm.o last-fault.o
# ALL_SOURCES := $(wildcard ./*.c)
OBJS := $(SYS_OBJS)
# $(patsubst %.c, %.o, $(ALL_SOURCES))  $(SYS_OBJS)
EXTRA_OBJS = our-mmu-asm.o
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

CFLAGS += -Wno-unused-function -Wno-unused-variable -fno-toplevel-reorder -Werror 

# -fno-omit-frame-pointer -mpoke

# -fno-reorder-functions

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(LIBM) $(wildcard ./*.h) ./Makefile 

# i *think* order is important.
LIBS = $(LPI)  

all:  libs $(OBJS) $(TEST_BINARIES) $(TEST_RUN) # $(TEST_CHECK)

update:
	cp part1-test*.c part1-test*.out ../code

libs: 
	make -C $(LPP) 

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: %.o $(OBJS) $(MEMMAP) $(DEPS) 
	$(LD) $(START) $*.o $(SYS_OBJS) $(EXTRA_OBJS) -T $(LPP)/memmap -o $@  $(LIBS)

%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

%.run: %.bin
	my-install ./$*.bin

# note: this will fail if output does not contain a trace.
# we pull everything into .out so that it's easier to debug.
%.emit: %.bin
	my-install ./$*.bin 2>&1 > /dev/null | cat > ./$*.out

%.check: %.bin
	my-install ./$*.bin 2>&1 > /dev/null | grep $(GREP_STR)  > ./$*.test
	grep $(GREP_STR) ./$*.out | diff ./$*.test -
	# diff ./$*.test ./$*.out
	rm ./$*.test

# not sure if there is a better way: use these fake target to run our
# checks for everything.
emitall: $(TEST_EMIT) $(TEST_BINARIES)
checkall: $(TEST_CHECK) $(TEST_BINARIES)
runall: $(TEST_RUN) 

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs] 

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags $(SYS_OBJS) *.bin *.elf *.list *.img Makefile.bak  *.d *.test
	make -C ./tests clean

cleanall:
	make -C ./ clean
	rm *.out

.PHONY: libs all tags clean run fake-libpi  runall emitall checkall update

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
