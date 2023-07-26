#ifndef IO_H_
#define IO_H_

#include <stdint.h>

void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);
void outw(uint16_t port, uint16_t data);
uint16_t inw(uint16_t port);
void outl(uint16_t port, uint32_t data);
uint32_t inl(uint16_t port);

#define outportb outb
#define inportb inb
#define outportw outw
#define inportw inw
#define outportl outl
#define inportl inl

#define ins inw
#define inports inw
#define outs outw
#define outports outw

void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port) :"memory");
}
void outw(uint16_t port, uint16_t data) {
    asm volatile ("outw %0, %1" : : "a"(data), "Nd"(port) :"memory");
}
void outl(uint16_t port, uint32_t data) {
    asm volatile ("outl %0, %1" : : "a"(data), "Nd"(port) :"memory");
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory");
    return ret;
}
inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ( "inw %1, %0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory");
    return ret;
}
inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ( "inl %1, %0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory");
    return ret;
}

#endif