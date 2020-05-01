// trivial program to reset the pins to off.
#include "rpi.h"

void notmain(void) {
    uart_init();

    unsigned up = 21, down = 20;

    gpio_pud_off(up);
    gpio_pud_off(down);
    printk("pin %d and %d are reset\n", up, down);
    clean_reboot();
}
