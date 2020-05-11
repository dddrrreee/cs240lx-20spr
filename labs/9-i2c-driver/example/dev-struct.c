// contrived, useful-only-for-illustration of how ot access
// fields
#include "rpi.h"

// uses macro hacks to define put32_T, get32_T and assign32_T
#include "libc/helper-macros.h"

void notmain(void) {
    volatile struct r_send {
        unsigned tx:1,   // tx=1 --> transmit
                 :15,
                 len:16;   // length of packet.
    } *r = (void*)0x12345678;
    _Static_assert(sizeof *r == 4, "invalid size of struct!");


    r->len = 8;
    r->tx = 1;

    // check that the bitfield is laid out how we thought.
    //             struct name   | field name |offset  | size (in bits)
    check_bitfield(struct r_send,  tx,         0,      1);
    check_bitfield(struct r_send,  len,        16,     16);
    

    /***********************************************************
     * method 1: have a pointer you never reference and use
     * get32_T and put32_T.  Guarentees a single load to get
     * the value and a single store to write it back.
     */

    struct r_send *addr = (void*)0x12345678;
    struct r_send s = get32_T(addr);

    // modify privately.
    s.len = 8;
    s.tx = 1;
    // flush out.
    put32_T(r,s);


    /***********************************************************
     * method 2: use an enum so can never dereference the pointer
     * raw.  same guarentees as above.
     */
    enum { dev_addr = 0x12345678 };
    assign32_T(s, dev_addr);

    // as before
    s.len = 8;
    s.tx = 1;
    // flush out.
    put32_T(r,s);
}

