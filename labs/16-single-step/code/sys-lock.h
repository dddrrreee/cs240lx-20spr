// should not be in this file, we do just to keep things simple.
typedef volatile int lock_t;

static inline void sys_lock_init(lock_t *l) { *l = 0; }

// returns 1 if lock success, 0 otherwise.
static inline int sys_lock_try(lock_t *l) {
    // define the routine in interrupts-asm.S
    int sys_try_lock_asm(lock_t *l);
    return sys_try_lock_asm(l);
}

static inline void sys_lock(lock_t *l) {
    while(!sys_lock_try(l))
        ;
    assert(*l == 1);
}
static inline void sys_unlock(lock_t *l) { 
    assert(*l == 1);
    *l = 0; 
}
