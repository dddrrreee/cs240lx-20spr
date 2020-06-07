// simple checker: A calls a system call, B does nothing: let's you 
// debug your system call.
#include "check-interleave.h"
#include "pi-sys-lock.h"

// trivial counter.   broken.
static volatile int cnt = 0;
int sys_foo_asm(void);
static void cnt_A(checker_t *c) { 
    if(sys_foo_asm() != 3)
        panic("impossible\n");
    cnt++;
} 

static int cnt_B(checker_t *c) { 
    // nothing.
    return 1;
}

static void cnt_init(checker_t *c) { 
    cnt = 0; 
}
static int  cnt_check(checker_t *c) { 
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
        panic("check should not have failed!\n");
    else 
        exit_success("check worked as it should have, ntrials=%d, nerrors=%d\n", 
                c.ntrials, c.nerrors);
}
