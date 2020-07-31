#ifndef __REFS_H__
#define __REFS_H__
// completely dumb resource tracking with reference counting.
// error if overflows.
// linear.
// pretty dumb.  
typedef uint8_t ref_t;

// allocate an unallocated reference
static inline int ref_alloc(ref_t *refs, unsigned nelem, int n) {
    if(n >= 0) {
        assert(n < nelem);
        if(refs[n])
            panic("trying to allocate entry %d, but alrady allocated\n", n);
        refs[n] = 1;
        return n;
    } else {
        for(int i = 0; i < nelem; i++) {
            if(!refs[i]) {
                refs[i] = 1;
                return i;
            }
        }
        panic("can't allocate!\n");
        return -1;
    }
}

// increment an allocated refernce
int ref_inc(ref_t *refs, unsigned nelem, unsigned i) {
    assert(i < nelem);
    if(!refs[i])
        panic("incrementing unallocated element %d\n", i);
    refs[i]++;

    if(!refs[i])
        panic("overflow: counter too small: wrapped around: %d\n", i);
    return refs[i];
}

// decrement an allocated refernce
int ref_dec(ref_t *refs, unsigned nelem, unsigned i) {
    assert(i < nelem);
    if(!refs[i])
        panic("decrementing 0 reference count %d\n", i);
    return --refs[i];
}

// error if it doesn't get a refcnt of 0.
void ref_free(ref_t *refs, unsigned nelem, unsigned i) {
    unsigned r = ref_dec(refs, nelem, i);
    if(r)
        panic("expected to free, but still have a reference.\n");
}

#endif
