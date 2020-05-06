// simple driver for hc-sr04
#include "rpi.h"
#include "hc-sr04.h"

// use this timeout(in usec) so everyone is consistent.
static unsigned timeout = 55000;

#ifdef RPI_UNIX
#   include "fake-pi.h"

#   ifdef FAKE_LOW_LEVEL

    // returns 1 if it decided on a value for <val>.  otherwise 0.
    int mem_model_get32(const volatile void *addr, uint32_t *val) {
        if(addr  == (void*)0x20003004) {
            unsigned t = fake_time_inc( fake_random() % (timeout * 2) );
            return t;
        }
        return 0;
    }

#   elif defined (FAKE_HIGH_LEVEL)

        unsigned timer_get_usec(void) {
            unsigned t = fake_time_inc( fake_random() % (timeout * 2) );
            trace("getting usec = %dusec\n", t);
            return t;
        }

#   else 

#       error "Impossible: must define FAKE_HIGH_LEVEL or FAKE_LOW_LEVEL"

#   endif

#endif


void notmain(void) {

    uart_init();

	printk("starting sonar!\n");
    hc_sr04_t h = hc_sr04_init(20, 21);
	printk("sonar ready!\n");

    for(int dist, i = 0; i < 10; i++) {
        // read until no timeout.
        while((dist = hc_sr04_get_distance(&h, timeout)) < 0)
            printk("timeout!\n");
        printk("distance = %d inches\n", dist);
        // wait a second
        delay_ms(1000);
    }
	printk("stopping sonar !\n");
    clean_reboot();
}
