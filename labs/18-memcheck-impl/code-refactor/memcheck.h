#ifndef __MEMCHECK_H__
#define __MEMCHECK_H__
#include "rpi.h"
#include "rpi-constants.h"
#include "rpi-interrupts.h"
#include "mmu.h"

// one-time initialization
void memcheck_init(void);

// start tracking.
void memcheck_on(void);
// stop
void memcheck_off(void);

// register the MB region [base,base+MB) so we start trapping on it.
void memcheck_track(uint32_t base);


// enable all memory trapping -- currently assumes using a single domain id.
void memcheck_trap_enable(void);

// disable all memory trapping -- currently assumes using a single domain id.
void memcheck_trap_disable(void);

// is trapping enabled?
unsigned memcheck_trap_enabled(void);


// XXX: hack to test that we can resume.
void memcheck_continue_after_fault(void);

int memcheck_fn(int (*fn)(void));

// just trace <fn>
int memtrace_fn(int (*fn)(void));

// for the moment, we just call these special allocator and free routines.
void *memcheck_alloc(unsigned n);
void memcheck_free(void *ptr);

void *sys_memcheck_alloc(unsigned n);
void sys_memcheck_free(void *ptr);

/**************************************************************
 * helper code: track information about the last fault.
 */

// a bit more general than memcheck, but we keep it here for simple.
typedef struct {
    uint32_t fault_addr,
             fault_pc,
             fault_reason,
             fault_count;
} last_fault_t; 

// returns the last fault
last_fault_t last_fault_get(void);
// check that the last fault was at pc, using addr with <reason>.  
//  if <fault_cnt> not zero, checks that.
void fault_expected(uint32_t pc, uint32_t addr, uint32_t reason, uint32_t fault_cnt);

#include "cpsr-util.h"

#endif
