// some tests for bit-support.h
//      gcc -O -Wall bit-test.c
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define output printf
#include "bit-support.h"

int main(void) {
    unsigned ntrials = 100000;

    assert(!bits_clr(~0, 0, 31));
    assert(bits_set(0, 0, 31, ~0) == ~0);
    assert(bits_get(~0, 0, 31) == ~0);
    assert(bits_eq(~0, 0, 31, ~0));
    assert(bits_set(~0, 0, 31, 0) == 0);
    assert(bits_set(~0, 0, 31, ~0) == ~0);

    for(int i = 0; i < 32; i++)
        bit_count(bits_mask(i)==i);

    for(int i = 0; i < ntrials; i++) {
        unsigned ub = random()%32;
        unsigned lb = random()%32;

        
        if(ub<lb)
            continue;

        unsigned n = ub-lb+1;
        unsigned v = (1 << n) - 1;
        if(!v) {
            assert(lb == 0 && ub == 31);
            v = ~0;
        }
        assert(v);

        unsigned x = bits_set(0, lb, ub, v);
        assert(bits_eq(x,lb,ub,v));
        assert(bits_get(x,lb,ub) == v);

        assert(bits_intersect(x,x) == x);
        assert(bits_diff(x,x) == 0);
        assert(bits_diff(x,0) == x);
        assert(bits_diff(0,x) == 0);
        assert(bits_union(x,x) == x);
        assert(bits_union(0,x) == x);

        for(unsigned i = lb; i <= ub; i++) {
            if(!bit_is_on(x,i))
                printf("lb=%d,ub=%d,x=%d, i=%d\n", lb,ub,x,i);
            assert(bit_is_on(x,i));
            assert(bits_get(x,i,i) == 1);
        }

        assert(bits_clr(x,lb,ub) == 0);

        for(unsigned i = lb; i <= ub; i++) {
            assert(x);
            x = bits_clr(x, i,i);
            assert(bits_clr(x, i,ub) == 0);
            assert(bits_clr(x, 0,i) == x);
            
            unsigned n = ub-lb+1;
            unsigned v1 = bits_mask(n);
            assert(bits_get(v1, 0, n-1) == v1);
            assert(bits_eq(v1, 0, n-1, v1));
            
            if(bit_count(v1) != n)
                output("bitcount(v1) == %d n=%d\n", bit_count(v1),n);
            assert(bit_count(v1) == n);

            if(ub<31)
                assert(bits_clr(v1, ub+1,31) == v1);

#if 0
            unsigned v2 = bits_set(0, 0, ub-lb, v1);
            assert(v1==v2);
            output("v=%x, ub=%u, clr=%x\n", v,ub,bits_clr(v,ub,31));
            assert(bits_clr(v2, ub,31) == v2);
            // assert(bits_set(0, i, ub, bits_mask(ub-lb+1)));
#endif
        }
        assert(!x);


    }
    return 0;
}
