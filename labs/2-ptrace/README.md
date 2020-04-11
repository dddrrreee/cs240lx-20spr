This lab will do some simple `ptrace` hacking.

Possible projects including using `ptrace` to:
 - implement `strace`.
 - cause system calls to fail in a traced program and make sure it does
   not blow up.
 - insert checks if a system call failed in a traced program.
 - observe a compilation and generate a `Makefile` + dependencies by tracing
    which include files are opened by the compiler when compiling a given 
    file.
