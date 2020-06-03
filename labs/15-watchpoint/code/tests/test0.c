// a not-super-thorough bit-routine checker.
#include "rpi.h"
#include "rpi-interrupts.h"
#include "libc/helper-macros.h"
#include "bit-support.h"

void notmain(void) {
    uint32_t u = ~0;

    assert(bits_eq(u, 0, 31, ~0));
    assert(bits_set(0, 0, 31, ~0) == u);
    assert(bits_clr(~0, 0, 31) == 0);

    for(int i = 0;  i < 32; i++) {
        assert(bit_isset(u,i));

        assert(bits_get(u,i, i) == 1);
        u = bit_clr(u,i);
        assert(bits_get(u,i, i) == 0);
        assert(!bit_isset(u,i));
        u = bit_set(u,i);
        assert(bit_isset(u,i));
        assert(bits_get(u,i, i) == 1);
    }

    unsigned short us = ~0;
    assert(bits_eq(us, 16, 31, 0));
    assert(bits_eq(us, 0, 15, us));
    printk("simple bit tests passed.\n");
}
