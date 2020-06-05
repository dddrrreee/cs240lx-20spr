#ifndef __CPSR_UTIL_H__
#define __CPSR_UTIL_H__

#include "rpi-interrupts.h"  // for cpsr_get, cpsr_set
#include "rpi-constants.h"  // for the modes.

static inline int mode_legal(unsigned mode) {
    return mode == USER_MODE
        || mode == IRQ_MODE
        || mode == SUPER_MODE
        || mode == ABORT_MODE
        || mode == SYS_MODE
    ;
}
static inline uint32_t mode_get(uint32_t cpsr) {
    unsigned mode = bits_get(cpsr, 0, 4);
    assert(mode_legal(mode));
    return mode;
}
static inline uint32_t mode_set(uint32_t cpsr, unsigned mode) {
    assert(mode_legal(mode));
    return bits_clr(cpsr, 0, 4) | mode;
}
static inline const char * mode_str(uint32_t cpsr) {
    switch(mode_get(cpsr)) {
    case USER_MODE: return "USER MODE";
    case FIQ_MODE:  return "FIQ MODE";
    case IRQ_MODE:  return "IRQ MODE";
    case SUPER_MODE: return "SUPER MODE";
    case ABORT_MODE: return "ABORT MODE";
    case UNDEF_MODE: return "UNDEF MODE";
    case SYS_MODE:   return "SYS MODE";
    default: panic("unknown mode: <%x>\n", cpsr);
    }
}
#endif
