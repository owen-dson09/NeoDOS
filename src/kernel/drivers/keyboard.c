#include "keyboard.h"
#include "../interrupts/pic.h"
#include "vga.h"
#include "util.h"

#define KEYBOARD_DATA_PORT 0x60

static const char scancode_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',

    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0, 'a', 's',

    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',

    'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0,
};

void keyboard_handler(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode & 0x80) { // key release
        return;
    }

    char c = 0;

    if (scancode < 128) {
        c = scancode_ascii[scancode];
    }

    if (c == 0) {
        return;
    }

    if (c == '\n') {
        vga_print("");
    } else if (c == '\b') {
        // backspace
    } else {
        char str[2];
        str[0] = c;
        str[1] = '\0';

        vga_write(str);
    }
}