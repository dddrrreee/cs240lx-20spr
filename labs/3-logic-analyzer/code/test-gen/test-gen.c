// engler, cs240lx: skeleton for test generation.  
#include "rpi.h"
#include "cs140e-src/cycle-count.h"
#include "cs140e-src/cycle-util.h"

#include "../scope-constants.h"

// send N samples at <ncycle> cycles each in a simple way.
void test_gen(unsigned pin, unsigned N, unsigned ncycle) {
    unsigned start = cycle_cnt_read();

    unimplemented();

    unsigned end = cycle_cnt_read();

    // crude check how accurate we were ourselves.
    printk("expected %d cycles, have %d\n", ncycle*N, end-start);
}

void notmain(void) {
    int pin = 21;
    gpio_set_output(pin);
    cycle_cnt_init();

    // keep it seperate so easy to look at assembly.
    test_gen(pin, 11, CYCLE_PER_FLIP);

    clean_reboot();
}
