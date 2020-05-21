.globl system_enable_interrupts
system_enable_interrupts:
    mrs r0,cpsr         @ move process status register (PSR) to r0
    bic r0,r0,#(1<<7)   @ clear 7th bit.
    msr cpsr_c,r0       @ move r0 back to PSR
    bx lr               @ return.

.globl system_disable_interrupts
system_disable_interrupts:
    mrs r0,cpsr
    orr r0,r0,#(1<<7)          @ set 7th bit: or in 0b100 0000
    msr cpsr_c,r0
    bx lr
