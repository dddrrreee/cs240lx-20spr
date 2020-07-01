/*
 * engler: cs340lx
 *
 * start of a memchecker.  sort of lame in that you have to deal with a bunch of 
 * my starter code (sorry) --- the issue is that it might not be clear how to 
 * call the vm.
 *
 * implement:
 *  - static void dom_perm_set(unsigned dom, unsigned perm) {
 *  - void data_abort_vector(unsigned lr) {
 * (1) is trivial, but just to remind you about domain id's.
 * (2) involves computing the fault reason and then acting.
 *
 */
#include "memcheck.h"
#include "cp14-debug.h"
#include "libc/helper-macros.h"

enum { OneMB = 1024 * 1024 };

// don't use dom id = 0 --- too easy to miss errors.
enum { dom_id = 1, track_id = 2 };

/**********************************************************************
 * helper code to track the last fault (used for testing).
 */
static last_fault_t last_fault;
last_fault_t last_fault_get(void) {
    return last_fault;
}
void last_fault_set(uint32_t fault_pc, uint32_t fault_addr, uint32_t reason) {
    last_fault = (last_fault_t) { 
            .fault_addr = fault_addr, 
            .fault_pc = fault_pc,
            .fault_reason = reason
    };
}
void fault_expected(uint32_t pc, uint32_t addr, uint32_t reason) {
    last_fault_t l = last_fault_get();
    assert(l.fault_addr == addr);
    assert(l.fault_pc ==  pc);
    assert(l.fault_reason == reason);
    trace("saw expected fault at pc=%p, addr=%p, reason=%d\n", pc,addr,reason);
}

/****************************************************************
 * mechanical code for flipping permissions for the tracked memory
 * domain <track_id> on or off.
 *
   from in the armv6.h header:
    DOM_no_access = 0b00, // any access = fault.
    DOM_client = 0b01,      // client accesses check against permission bits in tlb
    DOM_reserved = 0b10,      // client accesses check against permission bits in tlb
    DOM_manager = 0b11,      // TLB access bits are ignored.
 */

static unsigned dom_perm_get(unsigned dom) {
    unsigned x = read_domain_access_ctrl();
    return bits_get(x, dom*2, (dom+1)*2);
}

// set the permission bits for domain id <dom> to <perm>
// leave the other domains the same.
static void dom_perm_set(unsigned dom, unsigned perm) {
    assert(dom < 16);
    assert(perm == DOM_client || perm == DOM_no_access);

    unimplemented();
}


/**************************************************************************
 * handle a trap.
 */

// hack to turn off resume / not resume for testing.
static unsigned mmu_resume_p = 0;
void memcheck_continue_after_fault(void) {
    mmu_resume_p = 1;
}

// simple data abort handle: handle the different reasons for the tests.
void data_abort_vector(unsigned lr) {
    if(was_debug_datafault())
        mem_panic("should not have this\n");

    // b4-43
    unsigned dfsr = dfsr_get();
    unsigned fault_addr = far_get();

    // compute the rason.
    unsigned reason = 0;
    unimplemented();

    last_fault_set(lr, fault_addr, reason);

    /*
         for SECTION_XLATE_FAULT:
            trace_clean_exit("section xlate fault: addr=%p, at pc=%p\n" 
         for DOMAIN_SECTION_FAULT:
            if(!mmu_resume_p)
                trace_clean_exit("Going to die!\n");
            else {
                trace("going to try to resume\n");
                memcheck_trap_disable();
                return;
            }
        for SECTION_PERM_FAULT:
            panic("section permission fault: %x", fault_addr);

        otherwise
        default: panic("unknown reason %b\n", reason);
     */ 
}

// shouldn't happen: need to fix libpi so we don't have to do this.
void interrupt_vector(unsigned lr) {
    mmu_disable();
    panic("impossible\n");
}


/*************************************************************************
 * helper code: you don't have to write this; it's based on cs140e.
 */

// XXX: our page table, gross.
static fld_t *pt = 0;

// need some parameters for this.
void memcheck_init(void) {
    // 1. init
    mmu_init();
    assert(!mmu_is_enabled());

    void *base = (void*)0x100000;

    pt = mmu_pt_alloc_at(base, 4096*4);
    assert(pt == base);

    // 2. setup mappings

    // map the first MB: shouldn't need more memory than this.
    mmu_map_section(pt, 0x0, 0x0, dom_id);
    // map the page table: for lab cksums must be at 0x100000.
    mmu_map_section(pt, 0x100000,  0x100000, dom_id);
    // map stack (grows down)
    mmu_map_section(pt, STACK_ADDR-OneMB, STACK_ADDR-OneMB, dom_id);

    // map the GPIO: make sure these are not cached and not writeback.
    // [how to check this in general?]
    mmu_map_section(pt, 0x20000000, 0x20000000, dom_id);
    mmu_map_section(pt, 0x20100000, 0x20100000, dom_id);
    mmu_map_section(pt, 0x20200000, 0x20200000, dom_id);

    // if we don't setup the interrupt stack = super bad infinite loop
    mmu_map_section(pt, INT_STACK_ADDR-OneMB, INT_STACK_ADDR-OneMB, dom_id);

    // 3. install fault handler to catch if we make mistake.
    mmu_install_handlers();

    // 4. start the context switch:

    // set up permissions for the different domains: we only use <dom_id>
    // and permissions r/w.
    write_domain_access_ctrl(0b01 << dom_id*2);

    // use the sequence on B2-25
    set_procid_ttbr0(0x140e, dom_id, pt);
}

// turn memchecking on: right now, just enable the mmu (which should 
// flush any needed caches).
void memcheck_on(void) {
    debug("memcheck: about to turn ON\n");
    // note: this routine has to flush I/D cache and TLB, BTB, prefetch buffer.
    mmu_enable();
    assert(mmu_is_enabled());
    debug("memcheck: ON\n");
}
// turn memcheck off: right now just diable mmu (which should flush
// any needed caches)
void memcheck_off(void) {
    // 6. disable.
    mmu_disable();
    assert(!mmu_is_enabled());
    debug("memcheck: OFF\n");
}

void memcheck_map(uint32_t base) {
    assert(is_aligned(base,OneMB));

    // XXX: need to handle when it's already mapped.
    mmu_map_section(pt, base, base + OneMB, dom_id);
}

void memcheck_track(uint32_t base) {
    assert(is_aligned(base,OneMB));

    // XXX: need to handle when it's already mapped.
    mmu_map_section(pt, base, base + OneMB, track_id);
}

void memcheck_no_access(uint32_t base) {
    base &= ~(OneMB - 1);
    assert(is_aligned(base,OneMB));
    mmu_mark_sec_no_access(pt, base, 1);
}

// is trapping enabled?
unsigned memcheck_trap_enabled(void) {
    return dom_perm_get(track_id) == DOM_no_access;
}

// disable traps on tracked memory: we still do permission based checks
// (you can disable this using DOM_manager).
void memcheck_trap_disable(void) {
    dom_perm_set(track_id, DOM_client);
    assert(!memcheck_trap_enabled());
}
void memcheck_trap_enable(void) {
    dom_perm_set(track_id, DOM_no_access);
    assert(memcheck_trap_enabled());
}
