#include "rpi-asm.h"

// To keep this in the first portion of the binary.
.section ".text.boot"

#define prefetch_flush(reg) mov reg, #0; mcr p15, 0, r3, c7, c5, 4

.globl _start
_start:
    @ force the mode to be SUPER.
    mov r0,  #SUPER_MODE
    msr cpsr, r0
    prefetch_flush(r1)

    @ mov r0,  #0b10011
    @ mov r0, #0b11111


    mov sp, #STACK_ADDR
    mov fp, #0  // I don't think necessary.
    @ bl notmain
    bl _cstart
    bl rpi_reboot // if they return just reboot.


@ keep these here so that they are in the same location in every binary.
@ if you want to override them easily, can re-define the name in rpi-asm.h
@ can't seem to have multiple labels back to back?

.globl put32
.globl PUT32
put32:
PUT32:
    str r1,[r0]
    bx lr

.globl get32
.globl GET32

get32:
GET32:
    ldr r0,[r0]
    bx lr
