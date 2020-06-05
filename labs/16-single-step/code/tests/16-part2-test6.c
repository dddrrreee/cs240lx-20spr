// trivial check example: increments variable without having a lock.
// will fail.
#include "check-interleave.h"
#include "sys-lock.h"

// trivial counter.   broken.
static volatile int cnt = 0;
static lock_t l;
static volatile int B_retry = 0;

static void cnt_A(checker_t *c) { 
    if(!sys_lock_try(&l))
        panic("impossible\n");
    cnt++; 
    sys_unlock(&l);
} 
static void cnt_B(checker_t *c) { 
    if(!sys_lock_try(&l))
        B_retry = 1;
    else {
        cnt++; 
        sys_unlock(&l);
    }
}

static void cnt_init(checker_t *c) { 
    cnt = 0; 
    B_retry = 0;
    sys_lock_init(&l);
}
static int  cnt_check(checker_t *c) { 
    if(B_retry) {
        B_retry = 0;
        assert(l == 0);
        c->B(c);
        assert(!B_retry);
    }
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

    struct checker c = cnt_mk_checker();
    int n;
    if(check(&c))
        panic("check should have failed!\n");
    else 
        exit_success("check failed as it should have, ntrials=%d, nerrors=%d\n", 
                c.ntrials, c.nerrors);
}
