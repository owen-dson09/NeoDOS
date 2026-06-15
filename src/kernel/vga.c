#include "vga.h"

#define uint8_t unsigned char
#define uint16_t unsigned short

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_COL_WHITE_ON_BLACK 0x07

uint16_t* const vga_buffer = (uint16_t*)0xB8000;

void vclear() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            vga_buffer[index] = (uint16_t)' ' | (uint16_t)(VGA_COL_WHITE_ON_BLACK << 8);
        }
    }
}

void set_cursor_pos(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

uint16_t get_cursor_pos(void) {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

void get_cursor_xy(int *x, int *y) {
    uint16_t pos = get_cursor_pos();
    *x = pos % VGA_WIDTH;
    *y = pos / VGA_WIDTH;
}

void vputc(char c, unsigned char color, int x, int y) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return;
    const int index = y * VGA_WIDTH + x;
    vga_buffer[index] = (uint16_t)c | (uint16_t)(color << 8);
}

void vwrite(const char *str) {
    uint16_t pos = get_cursor_pos();
    int x = pos % VGA_WIDTH;
    int y = pos / VGA_WIDTH;

    for (int i = 0; str[i] != '\0'; i++) {
        if (x >= VGA_WIDTH) break;
        vputc(str[i], VGA_COL_WHITE_ON_BLACK, x, y);
        x++;
    }

    if (x >= VGA_WIDTH) {
        x = 0;
        y++;
    }

    set_cursor_pos(x, y);
}

void vprint(const char *str) {
    uint16_t pos = get_cursor_pos();

    for (int i = 0; str[i] != '\0'; i++) {
        vputc(str[i], VGA_COL_WHITE_ON_BLACK, pos % VGA_WIDTH, pos / VGA_WIDTH);
        pos++;
    }

    int y = (pos - 1) / VGA_WIDTH;
    set_cursor_pos(0, y + 1);
}