#include "rpi.h"
void notmain(void) {
    void hello(void);
    hello();
    printk("hello from new libpi!\n");
}
