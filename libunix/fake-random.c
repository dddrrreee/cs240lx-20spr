#include <memory.h>
#include "libunix.h"
#include "pi-random.h"

static unsigned ncalls = 0;

#define STATESIZE 128
static int seed = 0;
static char statebuf[STATESIZE];
static struct pi_random_data r;

void fake_random_seed(unsigned x) {
    ncalls = 1;
    memset(&r, 0, sizeof r);
    if(pi_initstate_r(seed, statebuf, STATESIZE, &r))
        assert(0);
    if(pi_srandom_r(seed, &r))
        assert(0);
}

// make sure that everyone has the same random.
unsigned fake_random_init(void) {
    fake_random_seed(0);
    unsigned u = fake_random();
    assert(0x6b8b4567 == u);
    return u;
}

unsigned long fake_random(void) {
    int x;
    if(pi_random_r(&r, &x))
        assert(0);

    trace("random called=%d times, value=%d\n", ncalls,x);
    ncalls++;
    return x;
}
