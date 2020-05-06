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
// call to move time forward.
unsigned fake_time_inc(unsigned inc);

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


// client memory model

// returns 1 if it decided on a value for <val>.  otherwise 0.
int mem_model_get32(volatile void *addr, uint32_t *val);

// performs any side-effect on <addr> using val, returns 0 if it
// did nothing.
int mem_model_put32(volatile void *addr, uint32_t val);


#endif
