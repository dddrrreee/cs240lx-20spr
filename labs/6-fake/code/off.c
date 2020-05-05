// turn the pullup/pulldown on pins {20,21} off so you don't get
// weird behavior later.
#include "rpi.h"

void notmain(void) {
    uart_init();

    unsigned up = 21, down = 20;

    gpio_pud_off(up);
    gpio_pud_off(down);
    printk("pin %d and %d are reset\n", up, down);
    clean_reboot();
}
