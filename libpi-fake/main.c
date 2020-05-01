// right now, just some wrapper code for random.
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "fake-pi.h"
#include "pi-random.h"

void dummy_fn(void) {}

int main(int argc, char *argv[]) {
    // don't handle any arguments at the moment.
    if(argc != 1) 
        die("usage error: %s [0..3]\n", argv[0]);

    // make stdout always flush.
    setvbuf(stdout, NULL, _IONBF, 0);

    unsigned u = fake_random_init();
    fake_time_init(u);

    notmain();
    return 0;
}
