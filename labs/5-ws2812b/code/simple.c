/*
 * Implement three parts for the lab:
 *  - part1: you can implement the routines in WS2812b.h and pass the timing checks.
 *  - part2: simple test to turn on the first pixel in the light string to blue.
 *  - part3: simple test to use your buffered neopixel interface to push a cursor around
 *    a light array.
 *  - part4: do some kind of interesting trick to light up the light array.
 */
#include "rpi.h"
#include "WS2812B.h"
#include "neopixel.h"

void check_timings(int pin);

// the pin used to control the light strip.
const unsigned pix_pin = 21;

// crude routine to write a pixel at a given location.
// XXX: do we have to do the membarriers?
void place_cursor(neo_t h, int i) {
    neopix_write(h,i-2,0xff,0,0);
    neopix_write(h,i-1,0,0xff,0);
    neopix_write(h,i,0,0,0xff);
    neopix_flush(h);
}

void notmain(void) {
    uart_init();

    // have to initialize the cycle counter or it returns 0.
    cycle_cnt_init();

    // output.
    gpio_set_output(pix_pin);

    // if you don't do this, the granularity is too large for the timing
    // loop. 
    enable_cache(); 

    // NOTE: when you get your code working, check these timings.
    // part 1: check your timings for t0l, t1h, t1l, t0l.
    check_timings(pix_pin);

    // part2: turn on one pixel to blue.
    // make sure you can:
    //  1. write different colors.
    //  2. write different pixels.
    pix_sendpixel(pix_pin, 0,0,0xff);
    pix_flush(pix_pin);
    delay_ms(1000*3);

    // part 3: make sure when you implement the neopixel 
    // interface works and pushes a pixel around your light
    // array.
    unsigned npixels = 60;  // you'll have to figure this out.
    neo_t h = neopix_init(pix_pin, npixels);
    unsigned start = timer_get_usec();
    while(1) {
        for(int j = 0; j < 10; j++) {
            for(int i = 0; i < npixels; i++) {
                place_cursor(h,i);
                delay_ms(10-j);
            }
        }
    }

    // part 4:   do some kind of interesting trick with your light strip.

    delay_ms(1000*3);
	clean_reboot();
}

// must be last so isn't inlined.
#include "timing-checks.h"
