// handle a load of 0.
#include "rpi.h"
#include "cp14-debug.h"

// put these here so they are at the same place in everyone's executable.
void test_put32(volatile void *u, uint32_t x) {
    *(volatile uint32_t *)u = x;
}
uint32_t test_get32(volatile void *u) {
    return *(volatile uint32_t *)u;
}

static void watchpt_handler(uint32_t regs[16], uint32_t pc, uint32_t addr) {
    output("client in watchpoint: from a %s addr=%p, pc=%p, adjusted wfar=%p\n", 
            datafault_from_ld() ? "ld" : "st", 
            addr,
            pc,
            wfar_get()-8);
    if(!datafault_from_ld())
        panic("test failed!\n");
    output("SUCCESS\n");
    clean_reboot();
}

void notmain(void) {
    cp14_enable();
    
    void *p = 0;
    watchpt_set0(0, watchpt_handler);
    printk("set watchpint for addr %p\n", p);

    printk("should see a load fault!\n");
    test_get32(p);
    panic("test failed!\n");
}
