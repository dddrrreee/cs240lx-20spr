#ifndef __PI_BITSUPPORT_H__
#define __PI_BITSUPPORT_H__
// some simple bit manipulation functions: helps make code clearer.

// set x[bit]=0 (leave the rest unaltered) and return the value
static inline 
uint32_t bit_clr(uint32_t x, unsigned bit) {
    assert(bit<32);
    return x & ~(1<<bit);
}

// set x[bit]=1 (leave the rest unaltered) and return the value
static inline 
uint32_t bit_set(uint32_t x, unsigned bit) {
    assert(bit<32);
    return x | (1<<bit);
}

// is x[bit] == 1?
static inline 
unsigned bit_isset(uint32_t x, unsigned bit) {
    assert(bit<32);
    return (x >> bit) & 1;
}

// extract bits [lb:ub]  (inclusive)
static inline 
uint32_t bits_get(uint32_t x, unsigned lb, unsigned ub) {
    assert(lb <= ub);
    assert(ub < 32);
    unsigned n = ub-lb+1;
    return ((x >> lb) & ((1<<n)-1));
}

// set bits[off:n]=0, leave the rest unchanged.
static inline 
uint32_t bits_clr(uint32_t x, unsigned lb, unsigned ub) {
    assert(lb <= ub);
    assert(ub < 32);
    unsigned n = ub-lb+1;

    unsigned mask = (1 << n) - 1;
    return x & (~mask << lb);
}

// set bits [lb:ub] to v (inclusive)
static inline 
uint32_t bits_set(uint32_t x, unsigned lb, unsigned ub, uint32_t v) {
    assert(lb <= ub);
    assert(ub < 32);
    unsigned n = ub-lb+1;
    assert((v >> n) == 0);

    return bits_clr(x, lb, ub) | (v << lb);
}

// bits [lb:ub] == <val?
static inline 
unsigned bits_eq(uint32_t x, unsigned lb, unsigned ub, uint32_t val) {
    assert(lb <= ub);
    assert(ub < 32);
    unsigned n = ub-lb+1;

    assert(val < (1ULL<<n));
    return bits_get(x, lb, ub) == val;
}

#endif
