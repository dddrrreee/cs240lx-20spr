#ifndef __FAKE_TEST_H__
#define __FAKE_TEST_H__
#include "libunix.h"

// simple random routines.

// get a random number
unsigned long fake_random(void);
// get a seed.
void fake_random_seed(unsigned x);
// initialization:
unsigned fake_random_init(void);

#define random "don't call random: use fake_random"



// setup the current "time".
void fake_time_init(unsigned init_time);

#define __RPI_ASSERT_H__
#include "rpi.h"

#include "demand.h"

// should use stderr, but it's irritating to capture the output.
#define trace(msg, args...) do {                                    \
    fprintf(stdout, "TRACE:%s:" msg, __FUNCTION__, ##args);          \
    fflush(stdout);                                                 \
} while(0)

void notmain(void);

extern unsigned fake_time_usec;

#endif
