/*
 * hall effect sensor: a1104eua-t (allegro microsystems)
 *
 * "The output of these devices (pin 3) switches low when the magnetic
 * field at the Hall element exceeds the operate point threshold (BOP). At
 * this point, the output voltage is VOUT(SAT). When the magnetic field
 * is reduced to below the release point threshold (BRP), the device
 * output goes high. The difference in the magnetic operate and release
 * points is called the hysteresis (Bhys) of the device. This built-in
 * hysteresis allows clean switching of the output even in the presence
 * of external mechanical vibration and electrical noise."
 *
 * Pay attention to the voltages on:
 *	- Vcc --- what is going into the device.
 *	- Vout --- what is coming out of the device.
 */
#include "rpi.h"
#include "hall.h"

// initialize the hall effect sensor.
//
//  1. recalL: you need to set GPIO pins as input or output before using them.
//  2. When circuit is "open" it can be noisy --- is it high or low when open?
//     If high need to configure a pin to be a "pullup", if low, "pulldown" to 
//     reject noise.  
//
hall_t hall_init_a1104(unsigned input_pin, unsigned debounce_cnt) {
    unimplemented();
}

// use gpio_read() to return 1 if magnet touching, 0 otherwise.
// only returns 1 if it has <debounce_cnt> reads of the same value.
unsigned hall_read(hall_t *h) {
    unimplemented();
}
