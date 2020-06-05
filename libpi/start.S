#include "rpi-asm.h"

// To keep this in the first portion of the binary.
.section ".text.boot"

#define prefetch_flush(reg) mov reg, #0; mcr p15, 0, r3, c7, c5, 4

.globl _start
_start:
    @ force the mode to be SUPER.
    mov r0,  #0b10011

    msr cpsr, r0
    prefetch_flush(r0)

    mov sp, #STACK_ADDR
    mov fp, #0  // I don't think necessary.
    @ bl notmain
    bl _cstart
    bl rpi_reboot // if they return just reboot.
