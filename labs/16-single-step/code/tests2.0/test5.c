// handle a jump to foo.
#include "rpi.h"
#include "cp14-debug.h"

void foo() { }

// breakpoint.
static void handler(uint32_t regs[16], uint32_t pc, uint32_t addr) {
    printk("prefetch abort at %p, foo = %p\n", pc, foo);
    if(!was_debug_instfault())
        panic("impossible: should get no other faults\n");
    assert((uint32_t)foo == pc);
    output("success!\n");
    clean_reboot();
}

void notmain(void) {
    cp14_enable();
    
    uint32_t p = (uint32_t)foo;
    brkpt_set0(p, handler);
    printk("set breakpoint for addr %p\n", p);

    printk("should see a breakpoint fault at %p!\n", p);
    BRANCHTO(p);
    panic("test failed!\n");
}
