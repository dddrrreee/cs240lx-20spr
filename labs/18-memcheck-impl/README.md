## Memcheck implementation

Today is a fetch-quest coding lab.  You're going to combine the pieces
from the last lab into a working memchecking system.   It's surprisingly
easy, as long as you don't make foolish mistakes (as I did).


#### Part 0: house-keeping

Last lab, we asked you to copy old files into your new project --- this was good in that
it wouldn't mess with working code, but it's obviously bad in that we have duplicate 
stuff everywhere.  What we should be doing in general is:
  1. Develop pieces in isolation.
  2. Test until we're happy.
  3. Commit the code to `libpi`.

We'll clean up a bit first.

I put the following in libpi:
 - `bit-support.h` into `libpi/libc`.
 - `cpsr-util.h` into `libpi/include`.

Delete these from:
  1. your `part1-single-step` and make sure it compiles.  
  2. your `part2-vm-trap` and make sure it compiles.

#### Part 1: `memcheck_fn`

If you look in `memcheck.h` there's a new function:

    int memcheck_fn(int (*fn)(void));

This will run `fn` in a checked environment that flags if you read or write unallocated or freed
memory in the heap.

If you look at the tests for part1 you can see they build up slowly: 
   - `part1-test0.c`: this just runs a routine without any checking.  Make sure this works before
     you start adding stuff.
   - `part1-test1.c`: this makes sure you are running at user level and that you can call a 
      system call.  Just make up a fake one that returns 10.

this just runs a routine without any checking.  Make sure this works before
 The first just runs a routine 

