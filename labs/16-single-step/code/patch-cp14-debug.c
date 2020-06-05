/**************************************************************
 * part 2
 */

int brk_verbose_p = 0;

// used by trampoline to catch if the code returns.
void brk_no_ret_error(void) {
    panic("returned and should not have!\n");
}

void brkpt_mismatch_set0(uint32_t addr, handler_t handler) {
    unimplemented();
}

// should be this addr.
void brkpt_mismatch_disable0(uint32_t addr) {
    unimplemented();
}

// <pc> should point to the system call instruction.
//      can see the encoding on a3-29:  lower 24 bits hold the encoding.
// <r0> = the first argument passed to the system call.
// 
// system call numbers:
//  <1> - set spsr to the value of <r0>
int syscall_vector(unsigned pc, uint32_t r0) {
    unimplemented();
}
