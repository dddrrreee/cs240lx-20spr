## Breakpoints

Most machines have some kind of breakpoint and watchpoint support, where
you can tell the hardware to give you an exception if a given address
is read, written or executed.  The arm chip we use is no exception.

AFAIK everyone in the lab has hit times where a random location in
memory got corrupted and you had to waste a bunch of time figuring out
what code did this.  After today's lab you should be able to detect such
corruption quickly:  
   1. Simply set a watchpoint on the address of the memory getting corrupted,
   2. When it occurs, you'll immediatle get an exception, where you can print out
      the program counter value doing it (or if you're fancy, a backtrace) along
      with any other information you would find useful.

Yes, we could do this with virtual memory, but that requires a lot more machinery.
The lab today should be around 100-200 lines of code, doesn't need virtual memory,
and if you continue to do OS or embedded stuff, will be very useful in the future.


So, unfortunately, the lab is cool, but I spent a bunch of time debugging something
(oh, the irony) so it's not written.  I'll keep pushing stuff out.  Don't worry,
this should be a relatively mechanical lab.


For the first thing, start reading chapter 13 of `docs/arm1176.pdf`.  You want
to see how to set:
  1. breakpoints
  2. watchpoints
  3. and how to descipher the exception.


As the first starting piece, write a routine to get the debug id register (13-6).
