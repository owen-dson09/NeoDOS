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

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

static inline void *uint_to_str(uint32_t value, char *buffer) {
    char temp[11];
    int i = 0;
    int j = 0;

    do {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    while (i > 0) {
        buffer[j++] = temp[--i];
    }

    buffer[j] = '\0';
    return buffer;
}

static inline char *int_to_str(int32_t value, char *buffer) {
    if (value < 0) {
        buffer[0] = '-';

        uint32_t magnitude = 0u - (uint32_t)value;
        uint_to_str(magnitude, buffer + 1);

        return buffer;
    }

    return uint_to_str((uint32_t)value, buffer);
}

#endif