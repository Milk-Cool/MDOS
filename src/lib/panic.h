#include <mdos.h>

void panic() {
    com_print(COM1, "KERNEL PANIC!!!\r\n");
    abort();
}