@ arguments:
@   r0 = the cpsr value to set.
@   r1 = the routine to call
MK_FN(user_trampoline_no_ret)
    @ put your code here.

    @ fail if they return.
    bl   brk_no_ret_error


@ arguments:
@   r0 = the cpsr value to set.
@   r1 = the routine to call
@   r2 = the handle pointer to pass to the call <r1>
@
@ also: move the current mode's stack pointer to the next mode's
@ sp
@
@ maybe we should just have a stack pointer switch routine?   the
@ trouble with this is that we need to do a system call to change
@ to super mode.  i don't know if this is the right thing.
MK_FN(user_trampoline_ret)
    @ put your code here.

    bx lr
