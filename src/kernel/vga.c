#include "vga.h"

void vga_print(const char *str) {
    unsigned char *vidmem = (unsigned char*)0xB8000;
    int i = 0;

    while (str[i] != '\0') {
        vidmem[i * 2] = str[i];         // Character
        vidmem[i * 2 + 1] = 0x07;       // Attribute (0x07 = white on black)
        i++;
    }
}

void set_cursor_pos(int x, int y) {
    unsigned short pos = y * 80 + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}