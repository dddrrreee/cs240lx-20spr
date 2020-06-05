// trivial check example: increments variable without having a lock.
// will fail.
#include "check-interleave.h"
#include "libc/circular.h"

// XXX: need to be able to yield back to compete a push.
static int expected_A, expected_B;
static cq_t cq;

// this is actually pretty tricky.
static void circq_A(checker_t *c) { 
    cq_push(&cq, 12);
    expected_A = 12;
}

static void circq_B(checker_t *c) { 
    cqe_t e;
    if(cq_pop_nonblock(&cq, &e)) 
        expected_B = e;
    else 
        // hasn't been pushed yet: need to yield here, pain otherwise.
        assert(expected_A == 0);
}
static void circq_init(checker_t *c) { 
    cq_init(&cq,0); 
    expected_A = expected_B = 0;
}

// what is the failure case?
static int  circq_check(checker_t *c) { 
    // if they match, good.
    if(expected_A == expected_B)
        return 1;

    // otherwise, the only valid thing is if B ran before there was
    // something.  rerun and make sure it works.
    c->B(c);

    assert(expected_A == expected_B);
    return 1;
}

checker_t circq_mk_checker(void) {
    return (struct checker) { 
        .state = 0,
        .A = circq_A,
        .B = circq_B,
        .init = circq_init,
        .check = circq_check
    };
}

void notmain(void) {
    // try with enable cache.
    enable_cache();

    struct checker c = circq_mk_checker();
    int n;
    if(!check(&c)) {
        assert(c.nerrors);
        panic("check should not have failed, trials=%d, errors=%d!!\n",
                c.ntrials, c.nerrors);
    } else {
        assert(!c.nerrors);
        exit_success("worked!  ntrials=%d\n", c.ntrials);
    }
}
