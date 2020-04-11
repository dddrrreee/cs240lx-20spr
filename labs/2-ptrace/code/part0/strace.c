// engler, starter code for trivial ptrace-based strace for cs240lx.
//  based on: https://nullprogram.com/blog/2018/06/23/
#include <unistd.h>
#include <stdlib.h>
#include <sys/user.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "libunix.h"

#include <sys/syscall.h>

const char *sysnum_to_name(int sysn) {
#   define sysnum(x) case x : return #x;

    switch(sysn) {
        sysnum(SYS_read);
        sysnum(SYS_write);
        sysnum(SYS_open);
        sysnum(SYS_close);
        sysnum(SYS_stat);
        sysnum(SYS_exit);
        sysnum(SYS_exit_group);
        sysnum(SYS_access);
        sysnum(SYS_mmap);
        sysnum(SYS_munmap);
        sysnum(SYS_mprotect);
        sysnum(SYS_brk);
        sysnum(SYS_arch_prctl);
        sysnum(SYS_fstat);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc < 2)
        panic("%s: expected at least 1 argument\n", argv[1]);

    pid_t pid;
    no_fail(pid = fork());

    if(!pid) {
        no_fail(ptrace(PTRACE_TRACEME));
        no_fail(execvp(argv[1], argv+1));
        panic("not reached\n");
    }
    // when returns, child is paused.  (XXX: or perhaps dead?)
    no_fail(waitpid(pid, 0,0));
    no_fail(ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL));

    for(int cnt = 0; ; cnt++) {
        no_fail(ptrace(PTRACE_SYSCALL, pid, 0, 0));
        no_fail(waitpid(pid, 0, 0));

        struct user_regs_struct regs;
        if(ptrace(PTRACE_GETREGS, pid, 0, &regs) < 0) {
            if(errno == ESRCH)
                clean_exit("child process pid=%d is dead: exiting\n", pid);
            sys_die(ptrace, "unexpected failure in ptrace");
        }

        
        long syscall = regs.orig_rax;
        if(!sysnum_to_name(syscall))
            panic("unknown call = %ld\n", syscall);

        // i think there is a bug since these repeat.
        long rdi = regs.rdi;
        if(rdi > 1024)
            output("syscall num %d = %s(0x%lx, ...)\n", 
                            cnt,
                            sysnum_to_name(syscall),
                            rdi);
        else 
            output("syscall num %d = %s(%ld, ...)\n", 
                            cnt,
                            sysnum_to_name(syscall),
                            rdi);

#if 0
        output("syscall num %d = (name=%s)= %ld(%ld, %ld, %ld, %ld, %ld, %ld)\n",
            cnt,
            sysnum_to_name(syscall),
            syscall,
            (long)regs.rdi, (long)regs.rsi, (long)regs.rdx,
            (long)regs.r10, (long)regs.r8,  (long)regs.r9);
#endif
    }
    return 0;
}
