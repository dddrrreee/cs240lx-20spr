// engler, cs240lx: simple scope skeleton for logic analyzer.
#include "rpi.h"
#include "cs140e-src/cycle-count.h"
#include "../scope-constants.h"

// dumb log.  use your own if you like!
typedef struct {
    unsigned v,ncycles;
} log_ent_t;


// compute the number of cycles per second
unsigned cycles_per_sec(unsigned s) {
    demand(s < 2, will overflow);
    unimplemented();
}

// monitor <pin>, recording any transitions until either:
//  1. we have run for about <max_cycles>.  
//  2. we have recorded <n_max> samples.
//
// return value: the number of samples recorded.
unsigned 
scope(unsigned pin, log_ent_t *l, unsigned n_max, unsigned max_cycles) {
    unimplemented();
}

// dump out the log, calculating the error at each point,
// and the absolute cumulative error.
void dump_samples(log_ent_t *l, unsigned n, unsigned period) {
    unsigned tot = 0, tot_err = 0;

    for(int i = 0; i < n-1; i++) {
        log_ent_t *e = &l[i];

        unsigned ncyc = e->ncycles;
        tot += ncyc;

        unsigned exp = period * (i+1);
        unsigned err = tot > exp ? tot - exp : exp - tot;
        tot_err += err;

        printk(" %d: val=%d, time=%d, tot=%d: exp=%d (err=%d, toterr=%d)\n", i, e->v, ncyc, tot, exp, err, tot_err);
    }
}

void notmain(void) {
    int pin = 21;
    gpio_set_input(pin);
    cycle_cnt_init();

#   define MAXSAMPLES 32
    log_ent_t log[MAXSAMPLES];

    unsigned n = scope(pin, log, MAXSAMPLES, cycles_per_sec(1));

    // <CYCLE_PER_FLIP> is in ../scope-constants.h
    dump_samples(log, n, CYCLE_PER_FLIP);
    clean_reboot();
}
