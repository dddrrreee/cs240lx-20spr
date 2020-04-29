/*
 */
#include "rpi.h"
#include "cycle-count.h"

void check_align(void);

void notmain(void) {
    uart_init();

    // have to initialize the cycle counter or it returns 0.
    cycle_cnt_init();

    // if you don't do this, the granularity is too large for the timing
    // loop. 
    enable_cache(); 

    check_align();
	clean_reboot();
}

/*
    8024:   eb000024    bl  80bc <printk>
    8028:   ee1f4f3c    mrc 15, 0, r4, cr15, cr12, {1}
    802c:   ee1f3f3c    mrc 15, 0, r3, cr15, cr12, {1}
    8030:   e0434004    sub r4, r3, r4
    8034:   e3540008    cmp r4, #8
    8038:   1a000006    bne 8058 <check_align+0x48>
 @ 803c % 16 = 12 --- so is in the last instuction in the 16 byte 
 @ prefetch buffer
    803c:   ee1f4f3c    mrc 15, 0, r4, cr15, cr12, {1}
 @ this will be in the next prefetch buffer.
    8040:   ee1f3f3c    mrc 15, 0, r3, cr15, cr12, {1}
    8044:   e0434004    sub r4, r3, r4
    8048:   e3540008    cmp r4, #8
    804c:   1a000009    bne 8078 <check_align+0x68>

 */
void check_align(void) { 
    unsigned b,e, overhead;

    asm volatile(".align 4");
    printk("starting\n");
    b = cycle_cnt_read();
    e = cycle_cnt_read();
    overhead = e-b;
    demand(overhead == 8, "expected 8, have overhead=%d\n", overhead);

    b = cycle_cnt_read();
    e = cycle_cnt_read();
    overhead = e-b;
    demand(overhead == 8, "expected 8, have overhead=%d\n", overhead);
}
