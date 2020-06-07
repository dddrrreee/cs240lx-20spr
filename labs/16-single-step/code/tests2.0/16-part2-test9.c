// trivial check example: increments variable without having a lock.
// will fail.
#include "check-interleave.h"
#include "pi-sys-lock.h"

// trivial counter.   broken.
static volatile int cnt = 0;
static pi_lock_t l;

static void cnt_A(checker_t *c) { 
    if(!sys_lock_try(&l))
        panic("impossible\n");
    cnt++; 
    sys_unlock(&l);
    
    assert(mode_get(cpsr_get()) == USER_MODE);
} 

// for the lab cswitch tester, we never get interrupted
static int cnt_B(checker_t *c) { 
    if(!sys_lock_try(&l))
        return 0;
    cnt++; 
    sys_unlock(&l);
    return 1;
}

static void cnt_init(checker_t *c) { 
    cnt = 0; 
    sys_lock_init(&l);
}
static int  cnt_check(checker_t *c) { 
    assert(cnt==2);
    return cnt == 2; 
}

checker_t cnt_mk_checker(void) {
    return (struct checker) { 
        .state = 0,
        .A = cnt_A,
        .B = cnt_B,
        .init = cnt_init,
        .check = cnt_check
    };
}

void notmain(void) {
    // try with enable cache.
    enable_cache();

    // need this to do syscalls.
    int_init();
    printk("mode = %s\n", mode_str(cpsr_get()));

    sys_lock_init(&l);
    assert(sys_lock_try(&l));
    for(int i = 0; i < 100; i++) {
        sys_lock_init(&l);
        assert(sys_lock_try(&l));
        assert(!sys_lock_try(&l));
        sys_unlock(&l);
        assert(sys_lock_try(&l));
    }

    struct checker c = cnt_mk_checker();
    int n;
    if(!check(&c))
        panic("check should not have failed!\n");
    else 
        exit_success("check succeeded as it should have, ntrials=%d, nerrors=%d\n", 
                c.ntrials, c.nerrors);
}
