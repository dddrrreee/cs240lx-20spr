#include "rpi.h"
#include "libc/helper-macros.h"
#include "ckalloc.h"

// simplistic heap management: a single, contiguous heap given to us by calling
// ck_init
static uint8_t *heap = 0, *heap_end, *heap_start;

void ck_init(void *start, unsigned n) {
    assert(aligned(heap_start, 8));
    printk("sizeof hdr=%d, redzone=%d\n", sizeof(hdr_t), REDZONE);
    heap = heap_start = start;
    heap_end = heap + n;
}

// simple access routines.

// pointer to first redzone
static void *b_rz1_ptr(hdr_t *h) { return (char *)h + sizeof *h; }
// address of user alloced space
static void *b_alloc_ptr(hdr_t *h) { return b_rz1_ptr(h) + REDZONE; }
// address of second redzone
static void *b_rz2_ptr(hdr_t *h) { return b_alloc_ptr(h) + h->nbytes_alloc; }
// includes the remainder
static unsigned b_rz2_nbytes(hdr_t *h) { return REDZONE + h->nbytes_rem; }

// given a pointer to the front of the allocated block, get the header.
static hdr_t *b_addr_to_hdr(void *addr) {
    return (void*) ((char *)addr - ALLOC_OFFSET);
}

// compute checksum on header.  need to do w/ cksum set to known value!
static uint32_t hdr_cksum(hdr_t *h) {
    unsigned old = h->cksum;
    h->cksum = 0;
    uint32_t cksum = fast_hash(h,sizeof *h);
    h->cksum = old;
    return cksum;
}

// check the header checksum and that its state == ALLOCED or FREED
static int check_hdr(hdr_t *h) {
    unimplemented();
}

static int check_mem(char *p, unsigned nbytes) {
    int i;
    for(i = 0; i < nbytes; i++) {
        if(p[i] != SENTINAL) {
            trace("block %p corrupted at offset %d\n", p, nbytes);
            return 0;
        }
    }
    return 1;
}
static void mark_mem(void *p, unsigned nbytes) {
    memset(p, SENTINAL, nbytes);
}

/*
 * check that:
 *  1. header is not corrupted (checksum passes).
 *  2. redzone 1 and redzone 2 are not corrupted.
 *
 */
static int check_block(hdr_t *h) {
    // short circuit the checks.
    return check_hdr(h)
        && check_mem(b_rz1_ptr(h), REDZONE)
        && check_mem(b_rz2_ptr(h), b_rz2_nbytes(h));
}

static void hdr_print(hdr_t *h) {
    src_loc_t *l = &h->alloc_loc;
    if(l->file)
        printk("[allocated @ %s:%s:%d]", l->file, l->func, l->lineno);

    l = &h->free_loc;
    if(h->state == FREED && l->file)
        printk("[freed @ %s:%s:%d]", l->file, l->func, l->lineno);
}

#define ck_error(_h, args...) do { \
    printk("TRACE:"); hdr_print(_h); printk(args); panic(args); } while(0)

/*
 *  give an error if so.
 *  1. header is in allocated state.
 *  2. allocated block does not pass checks.
 */
void (ckfree)(void *addr, const char *file, const char *func, unsigned lineno) {
    hdr_t *h = 0;

    demand(heap, not initialized?);
    trace("freeing %p\n", addr);
    unimplemented();
    assert(check_block(h));
}

// check if nbytes + overhead causes an overflow.
void *(ckalloc)(uint32_t nbytes, const char *file, const char *func, unsigned lineno) {
    hdr_t *h = 0;
    void *ptr = 0;

    demand(heap, not initialized?);
    trace("allocating %d bytes\n");

    unsigned tot = pi_roundup(nbytes, 8);
    unsigned n = tot + OVERHEAD_NBYTES;
    
    // this can overflow.
    if(n < nbytes)
        trace_panic("size overflowed: %d bytes is too large\n", nbytes);

    if((heap + n) >= heap_end)
        trace_panic("out of memory!  have only %d left, need %d\n", 
            heap_end - heap, n);



    unimplemented();
    assert(check_hdr(h));
    assert(check_block(h));

    trace("ckalloc:allocated %d bytes, (tot=%d), ptr=%p\n", nbytes, n, ptr);
    return ptr;
}

// integrity check the allocated / freed blocks in the heap
// if the header of a block is corrupted, just return.
// return the error count.
int ck_heap_errors(void) {
    unsigned alloced = heap - heap_start;
    unsigned left = heap_end - heap;

    trace("going to check heap: %d bytes allocated, %d bytes left\n", 
            alloced, left);
    unsigned nerrors = 0;
    unsigned nblks = 0;


    unimplemented();


    if(nerrors)
        trace("checked %d blocks, detected %d errors\n", nblks, nerrors);
    else
        trace("SUCCESS: checked %d blocks, detected no errors\n", nblks);
    return nerrors;
}

