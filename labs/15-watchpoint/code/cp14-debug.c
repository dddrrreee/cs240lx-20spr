// handle debug exceptions.
#include "rpi.h"
#include "rpi-interrupts.h"
#include "libc/helper-macros.h"
#include "cp14-debug.h"
#include "bit-support.h"

/******************************************************************
 * part1 set a watchpoint on 0.
 )*/
static handler_t watchpt_handler0 = 0;

// prefetch flush.
#define prefetch_flush() unimplemented()

// cp14_asm_get(wfar, c6, 0)
static inline uint32_t cp14_wcr0_get(void) {
    unimplemented();
}
static inline void cp14_wcr0_set(uint32_t r) {
    unimplemented();
}

static inline uint32_t cp14_wvr0_get(void) { unimplemented(); }
static inline void cp14_wvr0_set(uint32_t r) { unimplemented(); }

static inline uint32_t cp14_status_get(void) {
    unimplemented();
}

static inline void cp14_status_set(uint32_t v) {
    unimplemented();
}


// set the first watchpoint: calls <handler> on debug fault.
void watchpt_set0(void *_addr, handler_t handler) {
    unimplemented();

    prefetch_flush();
    watchpt_handler0 = handler;
}

// check for watchpoint fault and call <handler> if so.
void data_abort_vector(unsigned pc) {
    static int nfaults = 0;
    printk("nfault=%d: data abort at %p\n", nfaults++, pc);
    if(datafault_from_ld())
        printk("was from a load\n");
    else
        printk("was from a store\n");
    if(!was_debug_datafault()) 
        panic("impossible: should get no other faults\n");

    // this is the pc
    printk("wfar address = %p, pc = %p\n", wfar_get()-8, pc);
    assert(wfar_get()-4 == pc);

    assert(watchpt_handler0);
    
    uint32_t addr = far_get();
    printk("far address = %p\n", addr);

    // should send all the registers so the client can modify.
    watchpt_handler0(0, pc, addr);
}

void cp14_enable(void) {
    static int init_p = 0;

    if(!init_p) { 
        int_init();
        init_p = 1;
    }

    // for the core to take a debug exception, monitor debug mode has to be both 
    // selected and enabled --- bit 14 clear and bit 15 set.
    unimplemented();

    prefetch_flush();
    // assert(!cp14_watchpoint_occured());

}

/**************************************************************
 * part 2
 */

static handler_t brkpt_handler0 = 0;

static inline uint32_t cp14_bvr0_get(void) { unimplemented(); }
static inline void cp14_bvr0_set(uint32_t r) { unimplemented(); }
static inline uint32_t cp14_bcr0_get(void) { unimplemented(); }
static inline void cp14_bcr0_set(uint32_t r) { unimplemented(); }

static unsigned bvr_match(void) { return 0b00 << 21; }
static unsigned bvr_mismatch(void) { return 0b10 << 21; }

static inline uint32_t brkpt_get_va0(void) {
    return cp14_bvr0_get();
}
static void brkpt_disable0(void) {
    unimplemented();
}

// 13-16
// returns the 
void brkpt_set0(uint32_t addr, handler_t handler) {
    unimplemented();
}

// if get a breakpoint call <brkpt_handler0>
void prefetch_abort_vector(unsigned pc) {
    printk("prefetch abort at %p\n", pc);
    if(!was_debug_instfault())
        panic("impossible: should get no other faults\n");
    assert(brkpt_handler0);
    brkpt_handler0(0, pc, ifar_get());
}
