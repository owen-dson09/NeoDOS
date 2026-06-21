#include "keyboard.h"
#include "../interrupts/pic.h"
#include "vga.h"
#include "util.h"

#define KEYBOARD_DATA_PORT 0x60

static keyboard_callback_t keyboard_callback = 0;
static uint8_t shift_pressed = 0;

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

static const char scancode_ascii_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', '\b', '\t',

    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', '\n', 0, 'A', 'S',

    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0, '|', 'Z', 'X', 'C', 'V',

    'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' ', 0,
};

void keyboard_init(keyboard_callback_t callback) {
    pic_clear_mask(1); // Enable keyboard IRQ
    keyboard_callback = callback;
}

void keyboard_disable(void) {
    pic_set_mask(1); // Disable keyboard IRQ
}

void keyboard_handler(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode == 0x2A || scancode == 0x36) { // Shift pressed
        shift_pressed = 1;
        return;
    }

    if (scancode == 0xAA || scancode == 0xB6) { // Shift release
        shift_pressed = 0;
        return;
    }

    if (scancode & 0x80) { // Key release
        return;
    }

    char c = 0;

    if (scancode < 128) {
        if (shift_pressed) {
            c = scancode_ascii_shift[scancode];
        } else {
            c = scancode_ascii[scancode];
        }
    }

    if (c == 0) {
        return;
    }

    if (keyboard_callback) {
        keyboard_callback(c);
    }
}