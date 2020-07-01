// engler, test driver from lab 15 in 140e.  just makes sure
// that integration went ok --- should work out of the box with
// no changes.
#include "memcheck.h"

/* trivial vm test run.  */ 
void vm_test(void) {
    memcheck_init();
    memcheck_on();

    /*********************************************************************
     * trivial test: write a couple of values, make sure they succeed.
     *
     * you should write additional tests!
     */
    assert(mmu_is_enabled());

    // read and write a few values.
    int x = 5, v0, v1;
    assert(get32(&x) == 5);

    v0 = get32(&x);
    trace("doing print with vm ON\n");
    x++;
    v1 = get32(&x);

    /*********************************************************************
     * done with trivial test, re-enable.
     */

    memcheck_off();
    assert(!mmu_is_enabled());

    // 7. make sure worked.
    assert(v0 == 5);
    assert(v1 == 6);
    trace("******** success ************\n");
}

void notmain() {
    uart_init();
    check_vm_structs();
    vm_test();
    clean_reboot();
}
