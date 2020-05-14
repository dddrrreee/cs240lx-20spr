#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#
# We may make changes to it later, which would lead to 30+ conflicts.   So please don't
# modify it.

# how to make 'make' stop with a fatal error if warning?
MAKEFLAGS += --error-undefined-variables

# check that your path is defined.
ifndef CS240LX_2021_PATH
$(error CS240LX_2021_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif
include includes.mk

# if you get source from other directories, add them to vpath too.
VPATH := .:libc/:staff-src/:src:

# we used to compile everything in ./src but that caused a bunch of
# confusion.
# SOURCES     := $(wildcard ./libc/*.c  ./staff-src/*.c  ./src/*.c)
# ASM_SOURCES := $(wildcard ./libc/*.S  ./staff-src/*.S  ./src/*.S)

# this is where your code is.
include put-your-src-here.mk

# grab everything in libc and staff-src
SOURCES     := $(wildcard ./libc/*.c  ./staff-src/*.c ) $(YOUR_SRC)
ASM_SOURCES := $(wildcard ./libc/*.S  ./staff-src/*.S ) $(YOUR_ASM)
OUR_OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.S, %.o, $(ASM_SOURCES))

# make sure that people cannot skip errors.
CFLAGS += -Wno-unused-function -Werror

START= start
# is there a better way?
ifeq ($(USE_FP),1)
    BUILD_DIR := ./fp-objs
    TARGET := libpi-fp.a
    LIBM := libm
    STAFF_OBJS := $(foreach o, $(STAFF_OBJS), $(dir $o)/fp/$(notdir $o))
else
    BUILD_DIR := ./objs
    TARGET := libpi.a
endif

OBJS := $(OUR_OBJS) 
OBJS := $(foreach o, $(OBJS), $(BUILD_DIR)/$(notdir $o)) $(STAFF_OBJS)
# Get all depend files by replacing extensions.  we over dep on all headers.
DEPS := $(OBJS:.o=.d)  $(BUILD_DIR)/$(START).d 

EXTRA_DEPS = ./Makefile ./put-your-src-here.mk ./includes.mk

HDRS := $(wildcard ./*.h ./*/*.h) $(EXTRA_DEPS)

all: $(TARGET) $(START).o  $(LIBM)

libm:
	make -C libm USE_FP=1

# we bundle start.s with the library: just make different ones for each
# different start so that the library is internally consistent.
$(TARGET): $(OBJS) $(SUPPORT_OBJS) $(HDRS)
	arm-none-eabi-ar crf $(TARGET) $(OBJS) $(SUPPORT_OBJS)
	$(OD) -D $@ > $@.list

# kind of gross.  we want this .o at the top-level so people can link to it easily.
# so we need a duplicate rule.
$(START).o: $(BUILD_DIR)/$(START).d
	@mv $(BUILD_DIR)/$(START).o .

# XXX: We seem to need this rule to stop compilation with an error if the .c file
# has one: otherwise make does not stop compilation when generating .d.
# If compilation goes correctly, is not triggered.
$(BUILD_DIR)/%.o : %.c
$(BUILD_DIR)/%.o : %.c $(BUILD_DIR)/%.d $(EXTRA_DEPS)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

# XXX: I think we need this to catch errors in .S as well.
$(BUILD_DIR)/%.o : %.S
$(BUILD_DIR)/%.o : %.S $(BUILD_DIR)/%.d  $(EXTRA_DEPS)
	$(CC) $(CPP_ASFLAGS) $<

# Rules to automatically generate dependencies and put in build directory
# We want to recompile everything if the makefile changes.
$(BUILD_DIR)/%.d: %.c $(EXTRA_DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -MT $@ -MMD -MP -MF $@ $< -o $(BUILD_DIR)/$(notdir $*).o

$(BUILD_DIR)/%.d: %.S $(EXTRA_DEPS)
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

# *unbelievable*: make clean doesn't skip this include even though the 
# target is after.
#   https://www.gnu.org/software/make/manual/html_node/Goals.html
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

ctags: 
	ctags *.[chSs] */*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(TARGET) *~ $(TARGET).list tags start.o  fp-start.o
	make   --no-print-directory -C libc clean
	make   --no-print-directory -C staff-src clean
ifneq ($(USE_FP),1)
	make clean USE_FP=1
	make -C libm clean 
endif

.PHONY: clean all ctags libm
