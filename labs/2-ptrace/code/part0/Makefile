NAME = strace
CC = gcc

# check that your path is defined.
ifndef CS240LX_2021_PATH
$(error CS240LX_2021_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

LIBUNIX_PATH := $(CS240LX_2021_PATH)/libunix
LIBUNIX := $(LIBUNIX_PATH)/lib-support.unix.a
CFLAGS := -Wall -Werror -g -Wno-unused-function -Wno-unused-variable -I. -I$(LIBUNIX_PATH) -g -DRPI_UNIX

# just make all .o's depend on everything.  simple.
SOURCES := $(wildcard ./*.c)
HDRS := $(wildcard ./*.h)
DEPS := ./Makefile $(LIBUNIX) $(LIBUNIX_PATH)/libunix.h $(HDRS)

all : libs hello $(NAME) run

# make libunix quietly.  remove the '@' and the '-s' if you want to see 
# what is going on.
libs:
	make -s -C $(LIBUNIX_PATH)

hello: hello.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LIBUNIX) 

$(NAME): $(NAME).c $(LIBUNIX)  $(DEPS)
	$(CC) $(CFLAGS) $@.c -o $@ $(LIBUNIX) 

run:
	./$(NAME) ./hello

tags:
	ctags *.[ch] $(LIBUNIX_PATH)/*.[ch]

clean :
	rm -f *.o *.a *~ Makefile.bak $(NAME) hello

.PHONY: clean tags   libs run 
