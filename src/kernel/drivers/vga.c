#include "vga.h"
#include "util.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_DEFAULT_COL 0x07    // Light gray on black

uint16_t* const vgaBuffer = (uint16_t*)0xB8000;
static uint8_t textColor = VGA_DEFAULT_COL;

void setCursorPos(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

uint16_t getCursorPos(void) {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

void getCursorXY(int *x, int *y) {
    uint16_t pos = getCursorPos();
    *x = pos % VGA_WIDTH;
    *y = pos / VGA_WIDTH;
}

void setTextColor(uint8_t color) {
    textColor = (textColor & 0xF0) | color;
}

void setBackgroundColor(uint8_t color) {
    textColor = (textColor & 0x0F) | (color << 4);
}

void vga_clear() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            vgaBuffer[index] = (uint16_t)' ' | (uint16_t)(textColor << 8);
        }
    }
}

void vga_clear_line() {
    int pos = getCursorPos();
    int y = pos / VGA_WIDTH;

    for (int x = 0; x < VGA_WIDTH; x++) {
        const int index = y * VGA_WIDTH + x;
        vgaBuffer[index] = (uint16_t)' ' | (uint16_t)(textColor << 8);
    }
}

void vga_backspace(void) {
    uint16_t pos = getCursorPos();

    if (pos == 0)
        return;

    pos--;

    int x = pos % VGA_WIDTH;
    int y = pos / VGA_WIDTH;

    vga_putc(' ', textColor, x, y);
    setCursorPos(x, y);
}

void vga_scroll(int y) {
    if (y <= 0) return;

    if (y >= VGA_HEIGHT) {
        vga_clear();
        return;
    }

    int shift = y * VGA_WIDTH;
    int count = (VGA_HEIGHT - y) * VGA_WIDTH;

    for (int i = 0; i < count; i++) {
        vgaBuffer[i] = vgaBuffer[i + shift];
    }

    /* Clear newly exposed rows */
    for (int i = count; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vgaBuffer[i] = (uint16_t)' ' | (uint16_t)(textColor << 8);
    }
}

void vga_putc(char c, uint8_t color, int x, int y) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return;

    const int index = y * VGA_WIDTH + x;
    vgaBuffer[index] = (uint16_t)c | (uint16_t)(color << 8);
}

void vga_write(const char *str) {
    int x, y;
    getCursorXY(&x, &y);

    for (int i = 0; str[i] != '\0'; i++) {
        if (x >= VGA_WIDTH) break;
        vga_putc(str[i], textColor, x, y);
        x++;
    }

    setCursorPos(x, y);
}

void vga_wrapwrite(const char *str) {
    int x, y;
    getCursorXY(&x, &y);

    for (int i = 0; str[i] != '\0'; i++) {
        if (x >= VGA_WIDTH) break;
        vga_putc(str[i], textColor, x, y);
        x++;
    }

    if (x >= VGA_WIDTH) {
        x = 0;
        y++;

        if (y >= VGA_HEIGHT) {
            vga_scroll(1);
            y = VGA_HEIGHT - 1;
        }
    }

    setCursorPos(x, y);
}

void vga_print(const char *str) {
    int x, y;
    getCursorXY(&x, &y);

    for (int i = 0; str[i] != '\0'; i++) {
        vga_putc(str[i], textColor, x, y);
        x++;

        if (x >= VGA_WIDTH) {
            x = 0;
            y++;

            if (y >= VGA_HEIGHT) {
                vga_scroll(1);
                y = VGA_HEIGHT - 1;
            }
        }
    }

    x = 0;
    y++;

    if (y >= VGA_HEIGHT) {
        vga_scroll(1);
        y = VGA_HEIGHT - 1;
    }

    setCursorPos(0, y);
}