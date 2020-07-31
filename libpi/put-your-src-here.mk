 
# if you want to replace our .o with yours do as follows:
#   1. add yours to YOUR_OBJS
#   2. remove the corresponding from from STAFF_OBJS

# put the path to any .S files.
YOUR_ASM = 
# put the path to any .c files
YOUR_SRC = src/hello.c

# these are initial .o's we give you: to use your own, remove them
# and add yours (to YOUR_OBJS)
STAFF_OBJS= staff-objs/uart.o \
            staff-objs/gpio.o  \
            staff-objs/gpio-pud.o  \
            staff-objs/gpio-int.o  \
            staff-objs/kmalloc.o \
            staff-objs/sw-uart.o 
#           staff-objs/thread-asm.o    
#           staff-objs/uart-int.o 
#           staff-objs/rpi-thread.o   
