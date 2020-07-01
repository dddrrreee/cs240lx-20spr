#include "rpi.h"
#include "rpi-constants.h"
#include "rpi-interrupts.h"
#include "libc/helper-macros.h"
#include "mmu.h"

static const unsigned OneMB = 1024 * 1024 ;

static int verbose_p = 0;

int mmu_is_enabled(void) {
    return cp15_ctrl_reg1_rd().MMU_enabled != 0;
}

fld_t * mmu_lookup_section(fld_t *pt, unsigned va) {
    assert(is_aligned(va, 20));
    unsigned index = va >> 20;
    fld_t *pte = &pt[index];
    demand(pte->tag, invalid section);
    return pte;
}

static void 
mmu_mark_sec_ap_perm(fld_t *pt, unsigned va, unsigned nsec, unsigned perm) {
    va = mmu_sec_bits_only(va);
    demand(perm <= 0b11, invalid permission);
    for(int i = 0; i < nsec; i++) {

        // XXX: AH.  should not do this directly.  read in, write out.  can be in 
        // a weird state.
        fld_t *pte = mmu_lookup_section(pt, va);
        pte->AP = perm;
    }
    // have to kill everything.  not sure if this is even ok.
    our_mmu_sync_pte_mods();
}

void mmu_mark_sec_readonly(fld_t *pt, unsigned va, unsigned nsec) {
    mmu_mark_sec_ap_perm(pt,va,nsec, AP_rd_only);
}
void mmu_mark_sec_rw(fld_t *pt, unsigned va, unsigned nsec) {
    mmu_mark_sec_ap_perm(pt,va,nsec, AP_rw);
}
void mmu_mark_sec_no_access(fld_t *pt, unsigned va, unsigned nsec) {
    mmu_mark_sec_ap_perm(pt,va,nsec, AP_no_access);
}

// set f->sec_base_addr correctly.
static void fld_set_base_addr(fld_t *f, unsigned addr) {
    demand(is_aligned(addr,20), addr is not aligned!);
    // unimplemented();
    f->sec_base_addr = addr >> 20;

    // if the previous code worked, this should always succeed.
    assert((f->sec_base_addr << 20) == addr);
}

// create a mapping for <va> to <pa> in the page table <pt>
// for now: 
//  - assume domain = 0.
//  - global mapping
//  - executable
//  - don't use APX
//  - TEX,A,B: for today mark everything as non-cacheable 6-15/6-16 in armv1176
fld_t * mmu_map_section(fld_t *pt, uint32_t va, uint32_t pa, uint32_t dom) {
    assert(dom < 16);
    assert(is_aligned(va, 20));
    assert(is_aligned(pa, 20));

    unsigned index = va >> 20;
    fld_t *pte = &pt[index];
    demand(!pte->tag, already set);
    *pte = fld_mk();

    // hmm: if you want to alias our stuff, you would make this global.
    // possibly have them flip the most number of bits and have the same
    // result?
    pte->nG = 0;
    pte->APX = 0;
    pte->TEX = 0;
    pte->AP = AP_rw;
    pte->domain = dom;
    // everything can execute.
    pte->XN = 0;

    fld_set_base_addr(pte, pa);
    if(verbose_p) {
        fld_print(pte);
        printk("my.pte@ 0x%x = %b\n", pt, *(unsigned*)pte);
        hash_print("PTE crc:", pte, sizeof *pte);
    }
    return pte;
}

void mmu_map_sections(fld_t *pt, unsigned va, unsigned pa, unsigned nsec, uint32_t dom) {
    assert(dom < 16);
    assert(is_aligned(va, 20));
    assert(is_aligned(pa, 20));
    for(unsigned i = 0; i < nsec; i++) {
        mmu_map_section(pt, va,pa,dom);
        va += OneMB;
        pa += OneMB;
    }
}

fld_t *mmu_pt_alloc_at(void *addr, unsigned nbytes) {
    demand(is_aligned_ptr(addr, 14), must be 14-bit aligned!);
    fld_t *pt = addr;

    unsigned n = 4096 * sizeof *pt;
    demand(nbytes == n, only handing full allocation);

    memset(pt, 0, n);
    return pt;
}

fld_t *mmu_pt_alloc(unsigned n) {
    demand(n == 4096, we only handling a fully-populated page table right now);
    unsigned nbytes = 4096 * sizeof(fld_t);

    // first-level page table is 4096 entries.
    fld_t *pt = kmalloc_aligned(n, 1<<14);
    demand(is_aligned_ptr(pt, 14), must be 14-bit aligned!);

    return mmu_pt_alloc_at(pt, nbytes);
}

// set so that we use armv6 memory.
// this should be wrapped up neater.  broken down so can replace 
// one by one.
//  1. the fields are defined in vm.h.
//  2. specify armv6 (no subpages).
//  3. check that the coprocessor write succeeded.
void mmu_init(void) { 
    our_mmu_reset();

    struct control_reg1 c1 = cp15_ctrl_reg1_rd();
    c1.XP_pt = 1;
    cp15_ctrl_reg1_wr(c1);

    c1 = cp15_ctrl_reg1_rd();
    assert(c1.XP_pt);
    assert(!c1.MMU_enabled);
}

void mmu_install_handlers(void) {
    int_init();
}

// verify that interrupts are disabled.
void mmu_disable_set(cp15_ctrl_reg1_t c) {
    assert(!c.MMU_enabled);
    assert(!c.C_unified_enable);
    our_mmu_disable_set_asm(c);
}
void mmu_disable(void) {
    cp15_ctrl_reg1_t c = cp15_ctrl_reg1_rd();
    assert(c.MMU_enabled);
    c.MMU_enabled=0;
    mmu_disable_set(c);
}

void mmu_enable_set(cp15_ctrl_reg1_t c) {
    assert(c.MMU_enabled);
    our_mmu_enable_set_asm(c);
}
void mmu_enable(void) {
    cp15_ctrl_reg1_t c = cp15_ctrl_reg1_rd();
    assert(!c.MMU_enabled);
    c.MMU_enabled = 1;
    mmu_enable_set(c);
}

// b4-42
uint32_t read_domain_access_ctrl(void) {
    uint32_t r;
    asm volatile ("mrc p15, 0, %0, c3, c0, 0" : "=r" (r));
    return r;
}
void write_domain_access_ctrl(unsigned r) {
    our_cp15_domain_ctrl_wr(r);
}


// C end of this: does sanity checking then calls asm.
void set_procid_ttbr0(unsigned pid, unsigned asid, fld_t *pt) {
    assert((pid >> 24) == 0);
    assert(pid > 64);
    assert(asid < 64 && asid);
    our_cp15_set_procid_ttbr0(pid << 8 | asid, pt);
}

// hopefully catch if you call unimplemented stuff.
void asm_not_implemented(unsigned lr) {
    if(mmu_is_enabled())
        mmu_disable();
    panic("called unimplemented asm routine at %x\n", lr);
}
