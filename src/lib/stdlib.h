#ifndef STDLIB_H_
#define STDLIB_H_

void abort() {
    asm("cli");
    for (;;) {
        asm("hlt");
    }
}

#endif