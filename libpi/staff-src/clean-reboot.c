#include "rpi.h"

// print out a special message so bootloader exits
void clean_reboot(void) {
    reboot_handler();
    putk("DONE!!!\n");
    delay_ms(100);       // (hopefully) enough time for message to get flushed.
    rpi_reboot();
}
