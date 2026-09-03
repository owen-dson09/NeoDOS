#include "vga.h"
#include "util.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_DEFAULT_COL 0x07    // Light gray on black

uint16_t* const vga_buffer = (uint16_t*)0xB8000;
static uint8_t text_color = VGA_DEFAULT_COL;

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

void set_text_color(uint8_t color) {
    text_color = (text_color & 0xF0) | color;
}

void set_background_color(uint8_t color) {
    text_color = (text_color & 0x0F) | (color << 4);
}

void vga_clear() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            vga_buffer[index] = (uint16_t)' ' | (uint16_t)(text_color << 8);
        }
    }
}

void vga_clear_line() {
    int pos = get_cursor_pos();
    int y = pos / VGA_WIDTH;

    for (int x = 0; x < VGA_WIDTH; x++) {
        const int index = y * VGA_WIDTH + x;
        vga_buffer[index] = (uint16_t)' ' | (uint16_t)(text_color << 8);
    }
}

void vga_backspace(void) {
    uint16_t pos = get_cursor_pos();

    if (pos == 0)
        return;

    pos--;

    int x = pos % VGA_WIDTH;
    int y = pos / VGA_WIDTH;

    vga_putc(' ', text_color, x, y);
    set_cursor_pos(x, y);
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
        vga_buffer[i] = vga_buffer[i + shift];
    }

    /* Clear newly exposed rows */
    for (int i = count; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (uint16_t)' ' | (uint16_t)(text_color << 8);
    }
}

void vga_putc(char c, uint8_t color, int x, int y) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return;

    const int index = y * VGA_WIDTH + x;
    vga_buffer[index] = (uint16_t)c | (uint16_t)(color << 8);
}

void vga_write(const char *str) {
    int x, y;
    get_cursor_xy(&x, &y);

    for (int i = 0; str[i] != '\0'; i++) {
        if (x >= VGA_WIDTH) break;
        vga_putc(str[i], text_color, x, y);
        x++;
    }

    set_cursor_pos(x, y);
}

void vga_wrapwrite(const char *str) {
    int x, y;
    get_cursor_xy(&x, &y);

    for (int i = 0; str[i] != '\0'; i++) {
        if (x >= VGA_WIDTH) break;
        vga_putc(str[i], text_color, x, y);
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

    set_cursor_pos(x, y);
}

void vga_print(const char *str) {
    int x, y;
    get_cursor_xy(&x, &y);

    for (int i = 0; str[i] != '\0'; i++) {
        vga_putc(str[i], text_color, x, y);
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

    set_cursor_pos(0, y);
}