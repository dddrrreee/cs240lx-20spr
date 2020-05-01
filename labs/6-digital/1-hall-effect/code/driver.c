/* trivial driver for hall effect sensor */
#include "rpi.h"
#include "hall.h"

void notmain(void) {
    uart_init();

    printk("starting hall effect!\n");
    // first cross check with <0>, then use <3>.
    hall_t h = hall_init_a1104(21, 0);

    printk("ready: hall effect!\n");
    for(int i = 0; i < 10; i++) {
        while(hall_read(&h) == 0) {}
        printk("sensed magnet! count = %d\n", i);

        while(hall_read(&h) == 1) {}
        printk("no magnet!\n");
    }
    printk("stopping hall effect!\n");
    clean_reboot();
}
