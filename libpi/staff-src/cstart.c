#include "rpi.h"
#include "sw-uart.h"
#include "control-block.h"

#if 0
static sw_uart_t console_uart;
static int internal_sw_putchar(int c) {
    sw_uart_putc(&console_uart, c);
    return c;
}
#endif

#if 0
static void _cstart_reloc() {
#if 0
    extern char __data_start__, __data_end__, __data_lma__, __code_end__;
    char *data = &__data_start__;
    char *data_end = &__data_end__;
    char *code_end = &__code_end__;
    if(data != code_end)
        memcpy(data, &__data_lma__, data_end-data);
#else
    return;
    extern char __data_start__, __data_end__,__code_end__;
    char *data = &__data_start__;
    char *data_end = &__data_end__;
    char *code_end = &__code_end__;
    if(data != code_end)
        memcpy(data, code_end, data_end-data);

#endif
}
#endif

void _cstart() {
    extern int __bss_start__, __bss_end__;
	void notmain();

    custom_loader();

    int* bss = &__bss_start__;
    int* bss_end = &__bss_end__;
 
    while( bss < bss_end )
        *bss++ = 0;

#if 0
    _cstart_reloc();

#endif

#   ifdef RPI_FP_ENABLED
#   include "rpi-fp.h"
    enable_fpu();
    assert(fp_is_enabled);
#   endif

    cstart_handler();
#if 0
    // this links in so much stuff.  need to fix.
    control_blk_t *cb = cb_get_block();
    if(cb) {
        if(cb->console_dev == PUTC_SW_UART) {
            rpi_putchar = internal_sw_putchar;
            console_uart = cb->sw_uart;
            sw_uart_printk(&console_uart, "about to print on regular printf\n");
            printk("success!\n");
        }
    }
#endif
    notmain(); 
	clean_reboot();
}


