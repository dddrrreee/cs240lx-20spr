#ifndef __HALL_H__
#define __HALL_H__

// trivial hall effect interface.   we could keep the input 
// pin as a bare unsigned.  however, i think it makes sense 
// to have a standard interface for every device foo of:
//    foo_t foo_init(...);
// for input:
//    unsigned foo_read(foo_t *x);
// for output:
//    int foo_write(foo_t *x, unsigned v);

typedef struct hall {
    unsigned pin;

    // additional number of times we must see a 0 or 1 
    // before we have a stable reading.
    unsigned debounce;

    // anything else you might need to define.  e.g., for 
    // interrupt handling.
} hall_t;

// initialize a hall effect sensor that will use <pin>.  returns
// structure.  
hall_t hall_init_a1104(unsigned pin, unsigned debounce_cnt);

// return 1 if magnet touching, 0 otherwise.
unsigned hall_read(hall_t *hall);

#endif
