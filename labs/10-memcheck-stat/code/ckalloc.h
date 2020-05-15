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
    uint32_t nbytes_alloc;  // how much the user allocated.
    uint32_t nbytes_rem;    //the unused remainder.
    state_t state;              // state of the block.

    // checksum of this header struct: use fast_hash from our libpi/libc
    //  uint32_t fast_hash(&header, sizeof(header));
    uint32_t cksum;

    src_loc_t alloc_loc,    // location they called ckalloc() at.
              free_loc;     // location they called ckfree() at.

} hdr_t;

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
