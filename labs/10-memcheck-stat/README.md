## Statistical valgrind

***Make sure you do the [PRELAB](PRELAB.md)!***

### Part 0: some new functionality.

I put a new version of libpi in the top of our repository --- it was
getting a bit weird to modify the cs140e code.   You can put any code
you want to use in `libpi/src` and modify `put-your-src-here.mk`.
    - The example in `examples/hello` should run if you compile it.

I also set it up so you can use floating point.  
    - The example in `examples/float` should run if you compile it.

Its a hack where we compile `libpi` in two different ways (with float
and without).   The linker should give you an error if you link against
the wrong one.
