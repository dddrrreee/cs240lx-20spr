// fake memory.
//
// for the moment: simply print what put32 is writing, and return a random value for
// get32
#include "rpi.h"

unsigned get32(const volatile void *addr) {
    // you won't need this if you write fake versions of the timer code.
    // however, if you link in the raw timer.c from rpi, thne you'll need
    // to recognize the time address and returns something sensible.
    unimplemented();
}

// for today: simply print (addr,val) using trace()
void put32(volatile void *addr, unsigned val) {
    unimplemented();
}
