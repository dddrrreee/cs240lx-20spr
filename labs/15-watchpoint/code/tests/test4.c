// handle a jump to 0.
#include "rpi.h"
#include "cp14-debug.h"

// breakpoint.
static void handler(uint32_t regs[16], uint32_t pc, uint32_t addr) {
    printk("prefetch abort at %p\n", pc);
    if(!was_debug_instfault())
        panic("impossible: should get no other faults\n");
    assert(pc == 0);
    output("success!\n");
    clean_reboot();
}

void notmain(void) {
    cp14_enable();
    
    uint32_t p = 0;
    brkpt_set0(p, handler);
    printk("set breakpoint for addr %p\n", p);

    printk("should see a breakpoint fault at %p!\n", p);
    BRANCHTO(p);
    panic("test failed!\n");
}
