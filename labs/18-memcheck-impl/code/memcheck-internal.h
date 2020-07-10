#ifndef __MEMCHECK_INTERNAL_H__
#define __MEMCHECK_INTERNAL_H__

enum {
    SH_INVALID     = 1 << 0,
    SH_ALLOCED     = 1 << 1,      // writeable, allocated
    SH_FREED       = 1 << 2,
    SH_INITIALIZED = 1 << 3,
    SH_READ_ONLY   = 1 << 4,
    SH_UNINIT      = 1 << 5,
};

typedef struct __attribute__ ((packed)) {
    unsigned char state;
} state_t;

_Static_assert((SH_INVALID ^ SH_ALLOCED ^ SH_FREED ^ SH_INITIALIZED
                ^ SH_READ_ONLY ^ SH_UNINIT) == 0b111111, 
                "illegal value: must not overlap");

static inline int sh_is_set(state_t v, unsigned state) {
    return (v.state & state) == state;
}
static inline int sh_is_alloced(state_t s) { return sh_is_set(s, SH_ALLOCED); }
static inline int sh_is_freed(state_t s) { return sh_is_set(s, SH_FREED); }

#endif
