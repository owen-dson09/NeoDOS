#ifndef UTIL_H
#define UTIL_H

#define int8_t char
#define int16_t short
#define int32_t int
#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

#endif