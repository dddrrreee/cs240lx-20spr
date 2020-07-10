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

#### Part 1: running a routine at user-level: `memcheck_fn`

If you look in `memcheck.h` there's a new function:

    int memcheck_fn(int (*fn)(void));

This will run `fn` in a checked environment that flags if you read or
write unallocated or freed memory in the heap.  For part 1 we are just
going to run it at user level with the heap protected --- as soon as you
get a single fault, disable checking and continue.

The first time it is called (but only the first time!) it should do
any initialization:

   1. Call `memcheck_init` 
   2. Call `single_step_init`.  
   3. Map the heap at  `(uint32_t)pt + OneMB`
       (I think other places should work fine; this just makes it easy for
       us to compare values.)
   4. Map the shadow memory at one MB after the heap (other places work fine too).

Note: the way we are doing this --- by sticking stuff at random constants
---- is really lame and done only for expediency.  The one minor thing
that makes it better is that the MMU code will warn if we conflict and
the MMU itself will throw a fault if we reference memory we did not map.
Longer term we need something less embarassing.

The rest of the routine should:
   1. Turn on mem-checking
   2. call `fn` using your `user_mode_run_fn` routine.
   3. Turn off mem-checking.
   4. Return.


If you look at the tests for part1 you can see they build up slowly: 
   - `part1-test0.c`: this just runs a routine without any checking.
      Make sure this works before you start adding stuff.  Run it first without
      any of the initialization above, then add the init and make sure it still
      works.

   - `part1-test1.c`: this makes sure you are running at user level and
      that you can call a system call.  Just make up a fake one that
      returns 10.

   - `part1-test2.c`: this makes sure you are running at user level and
      that you can take a single domain trap.    For this part you'll have
      to protect the heap and, in the trap handler, change the permissions
      for the tracked domain.  Sadly, it has no internal checking so
      you'll have to look at the output to make sure it makes sense.

#### Part 2: trapping each load and store.
