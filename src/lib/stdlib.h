#ifndef STDLIB_H_
#define STDLIB_H_

#include <alloc.h>

void abort() {
    asm("cli");
    for (;;) {
        asm("hlt");
    }
}
void exit() {
    abort(); // FIXME: not abort
}

#endif