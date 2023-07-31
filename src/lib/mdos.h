#ifndef MDOS_H_
#define MDOS_H_

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8
#define COM5 0x5F8
#define COM6 0x4F8
#define COM7 0x5E8
#define COM8 0x4E8

#include <io.h>
#include <string.h>

#define UNUSED(x) (void)(x)

int com_init(uint16_t com_port) {
    outb(com_port + 1, 0x00);
    outb(com_port + 3, 0x80);
    outb(com_port + 0, 0x03);
    outb(com_port + 1, 0x00);
    outb(com_port + 3, 0x03);
    outb(com_port + 2, 0xC7);
    outb(com_port + 4, 0x0B);
    outb(com_port + 4, 0x1E);
    outb(com_port + 0, 0xAE);

    if(inb(com_port + 0) != 0xAE) {
        return 1;
    }

    outb(com_port + 4, 0x0F);
    return 0;
}

int com_transmit_empty(uint16_t com_port) {
   return inb(com_port + 5) & 0x20;
}

void com_print(uint16_t com_port, const char* str) {
    while (com_transmit_empty(com_port) == 0);
    for(uint32_t i = 0; i < strlen(str); i++)
        outb(com_port, str[i]);
}

typedef struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} register_t;

#endif