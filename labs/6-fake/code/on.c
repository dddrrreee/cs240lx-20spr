// set pin 21 to a pullup, 20 to a pulldown.
#include "rpi.h"

void notmain(void) {
    uart_init();

    unsigned up = 21, down = 20;

    // i don't believe we need to set input (since the state is sticky)
    gpio_set_input(up);
    gpio_set_pullup(up);
    printk("pullup=%d, this is sticky: touch red lead to this pin and black to ground: should see a ~3V reading on multimeter\n", up);

    gpio_set_input(up);
    gpio_set_pulldown(down);
    printk("pulldown=%d, this is sticky: touch black lead to this pin, and red to 3v: should see a ~3V reading on multimeter\n", down);

    clean_reboot();
}
