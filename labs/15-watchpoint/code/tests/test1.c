// part0 debug test.
#include "rpi.h"

#include "libc/helper-macros.h"     // to check the debug layout.
#include "bit-support.h"           // bit_* and bits_* routines.
#include "cp14-debug.h"             // our debug code.

void notmain(void) {

    // sanity check the structure.
    check_bitfield(struct debug_id, revision, 0, 4);
    check_bitfield(struct debug_id, variant, 4, 4);
    check_bitfield(struct debug_id, debug_rev, 12, 4);
    check_bitfield(struct debug_id, debug_ver, 16, 4);
    check_bitfield(struct debug_id, context, 20, 4);
    check_bitfield(struct debug_id, brp, 24, 4);
    check_bitfield(struct debug_id, wrp, 28, 4);


    // 13-6: get the debug id register value
    uint32_t r = cp14_debug_id_get();
    struct debug_id d = *(struct debug_id *)&r;

    // sanity check it.
    printk("r=%x\n", r);
    assert(r>>12 == 0x15121);
    assert(bits_get(r, 12, 31) == 0x15121);

    // 13-7: number of watchpoint regs should be 2 (+1 the actual value)
    assert(bits_get(r, 28,31) == 0b001);
    assert(d.wrp == 0b1);
    printk("have %d watchpoints\n", d.wrp+1);

    // 13-7: number of breakpoint regs should be 6 (+1 the actual value)
    assert(bits_get(r, 24, 27) == 5);
    assert(d.brp == 0b101);
    printk("have %d breakpoints\n", d.brp+1);

    printk("SUCCESS: read debug register correctly.\n");
}
