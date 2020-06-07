// trivial check: A calls lock, B does nothing.
// helps you debug your trylock implementation.
#include "check-interleave.h"
#include "pi-sys-lock.h"

static volatile int cnt = 0;
static pi_lock_t l;

unsigned sys_foo_asm(void);
void print_lr(unsigned lr) {
    panic("lr =%p\n",lr);
}

static void cnt_A(checker_t *c) { 
    if(!sys_lock_try(&l))
        panic("impossible\n");
    cnt++; 
    sys_unlock(&l);
} 

static int cnt_B(checker_t *c) { 
    // nothing.
    return 1;
}

static void cnt_init(checker_t *c) { 
    cnt = 0; 
    sys_lock_init(&l);
}
static int  cnt_check(checker_t *c) { 
    assert(l == 0);
    assert(cnt == 1);
    return 1;
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
    if(!check(&c))
        panic("check not should have failed!\n");
    else 
        exit_success("check worked as it should have, ntrials=%d, nerrors=%d\n", 
                c.ntrials, c.nerrors);
}
