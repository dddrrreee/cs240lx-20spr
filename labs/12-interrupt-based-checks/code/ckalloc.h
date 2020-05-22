#ifndef __CKALLOC_H__
#define __CKALLOC_H__

/***********************************************************
 * lab 10.
 */

// call first, before anything else.
//  - <heap_start> is where the heap starts.  must be divisible by 8.
//  - <nbytes> is the number of bytes in the heap.
void ck_init(void *heap_start, unsigned nbytes);

// pass the file, line, function of caller into ckalloc/ckfree.
//  * should extend to use backtraces.
#define ckalloc(_n) (ckalloc)(_n, __FILE__, __FUNCTION__, __LINE__)
#define ckfree(_ptr) (ckfree)(_ptr, __FILE__, __FUNCTION__, __LINE__)
void *(ckalloc)(uint32_t nbytes, const char *file, const char *func, unsigned lineno);
void (ckfree)(void *addr, const char *file, const char *func, unsigned lineno);


// integrity check the allocated / freed blocks in the heap
//
// returns number of errors in the heap.   stops checking
// if heap is in an unrecoverable state.
//
// TODO (extensions): 
//  - probably should have returned an error log, so that the 
//    caller could report / fix / etc.
//  - give an option to fix errors so that you can keep going.
int ck_heap_errors(void);

/***********************************************************
 *
 * lab 11
 */

// public function: call to flag leaks (part 1 of lab).
//  - <warn_no_start_ref_p>: if true, will warn about blocks that only have 
//    internal references.
//
//  - returns number of bytes leaked (where bytes = amount of bytes the
//    user explicitly allocated: does not include redzones, header, etc).
unsigned ck_find_leaks(int warn_no_start_ref_p);

// mark and sweep: works similarly to ck_find_leaks, resets
// state of unrefernced blocks.
// 
// Invariant:
//  - it should always be the case that after calling ck_gc(), 
//    immediately calling ck_find_leaks() should return 0 bytes 
//    found.
unsigned ck_gc(void);

// These two routines are just used for testing:

// Expects no leaks.
void check_no_leak(void);
// Expects leaks.
unsigned check_should_leak(void);


/************************************************************
 * lab 12
 */

// initialize interrupt checking.
void ck_mem_init(void);

// turn int-mem checking on.
void ck_mem_on(void);
// turn int-mem checking off.
void ck_mem_off(void);

// limit checking to the pc addresses in [start,end)
void ck_mem_set_range(void *start, void *end);

// returns 1 if <pc> is in a range that should be checked,
// 0 otherwise.
int (ck_mem_checked_pc)(uint32_t pc);

// hack so you don't have to keep casting everywhere.
#define ck_mem_checked_pc(_x) (ck_mem_checked_pc)((uint32_t)_x)

// dump out stats about checking.  if <clear_stats_p>=1, then 
// reset them.
unsigned ck_mem_stats(int clear_stats_p);

#endif
