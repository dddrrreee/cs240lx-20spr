/*
 * Simple debugging memory allocator. 
 *
 * Each memory block is laid out as follows:
 *
 *   +--------+-----------------+------+------------------+-------------------+
 *   | header | redzone         | data | unused remainder | redzone           |
 *   +--------+-----------------+------+------------------+-------------------+
 *
 * Remainders arise when a memory request for a given size is satisfied with 
 * a larger block.  ``remainder + tail fence post'' can be derived using
 * block_sz - data_sz - sizeof(header) - FENCE_SZ;
 *
 * Simplistic assumptions --- these make sense to change, just not in today's
 * lab:
 *  - all blocks are 8-byte aligned and a multiple of 8 bytes.
 *  - zero filled on allocation.
 *  - redzones are the same size.
 *  - redzones and remainders are filled with a fixed byte (can extend to hash
 *    using the address).
 *  - the header stores block size and allocation/free site (file, function, lineno).
 *  - when users call free() we mark the block as free but do not reuse it 
 *    (to minimize chance reuse can cause us to miss errors).  this also 
 *    simplifies the implementation.
 *
 *  - all memory is laid out in a single contiguous region.
 *      +----+----+----+--------------------+
 *      | b1 | b2 | b3 | ......  free ....  |
 *      +----+----+----+--------------------+
 *    cannot be resized.  to allocate take a piece from the front.  when you 
 *    run out, you panic.
 *
 *    easy to enumerate all blocks using the header information.
 */

typedef enum {  ALLOCED = 11, FREED } state_t;

typedef struct {
    const char *file;
    const char *func;
    unsigned lineno;
} src_loc_t;

// pull the remainder into the second redzone.
typedef struct header {
    uint32_t nbytes_alloc;  // how much the user requested to allocate.
    uint32_t nbytes_rem;    // the unused remainder (since we roundup).
    uint32_t state;          // state of the block: { ALLOCED, FREED }

    // checksum of this header struct: use fast_hash from our libpi/libc
    //  uint32_t fast_hash(&header, sizeof(header));
    uint32_t cksum;

    src_loc_t alloc_loc,    // location they called ckalloc() at.
              free_loc;     // location they called ckfree() at.
                            // used for error reporting.

    // used for gc
    uint32_t refs_start;    // number of pointers to the start of the block.
    uint32_t refs_middle;   // number of pointers to the middle of the block.

    // if you do a coalescing free you need something like this.
    struct header  *prev;   // pointer to previous block, if any.  needed if
                            //     we do a coalescing free.
    uint32_t padding;       // 8 byte align.
} hdr_t;


// shouldn't be in the header: emit error, don't panic
#define ck_error(_h, args...) do {      \
        trace("ERROR:");\
        printk(args);                    \
        hdr_print(_h);                  \
} while(0)

// emit error, then panic.
#define ck_panic(_h, args...) do {      \
        trace("ERROR:");\
        printk(args);                    \
        hdr_print(_h);                  \
        panic(args);                    \
} while(0)

// shouldn't be in the header.
static void inline hdr_print(hdr_t *h) {
    trace("\tnbytes=%d\n", h->nbytes_alloc);
    src_loc_t *l = &h->alloc_loc;
    if(l->file)
        trace("\tBlock allocated at: %s:%s:%d\n", l->file, l->func, l->lineno);

    l = &h->free_loc;
    if(h->state == FREED && l->file)
        trace("\tBlock freed at: %s:%s:%d\n", l->file, l->func, l->lineno);
}


// shouldn't be in the header.
enum {
    SENTINAL = 0xfe,      // the sentinal we mark with --- should be less naive.

    REDZONE = 128,      // sizeof redzone

    // offset of user allocation.
    ALLOC_OFFSET = sizeof(hdr_t) + REDZONE,
    OVERHEAD_NBYTES = ALLOC_OFFSET + REDZONE
};

// to make things easy in lab; trivial to fix.
_Static_assert((sizeof(hdr_t) + REDZONE) % 8 == 0, "REDZONE + header must be 8 byte aligned!");



#define ckalloc(_n) (ckalloc)(_n, __FILE__, __FUNCTION__, __LINE__)
#define ckfree(_ptr) (ckfree)(_ptr, __FILE__, __FUNCTION__, __LINE__)

void *(ckalloc)(uint32_t nbytes, const char *file, const char *func, unsigned lineno);
void (ckfree)(void *addr, const char *file, const char *func, unsigned lineno);

void ck_init(void *heap_start, unsigned n);

// integrity check the allocated / freed blocks in the heap
//
// returns number of errors in the heap.   stops checking
// if heap is in an unrecoverable state.
int ck_heap_errors(void);
