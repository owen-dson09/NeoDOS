#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "idt.h"
#include "interrupts/pic.h"
#include "util.h"

static void vga_key_press(char c) {
    if (c == '\n') {
        vga_print("");
    } else if (c == '\b') {
        vga_backspace();
    } else {
        char str[2];
        str[0] = c;
        str[1] = '\0';

        vga_wrapwrite(str);
    }
}

void main() {
    idt_init();
    pic_remap();

    // Mask PIC (ignore hardware interrupts)
    for (int i = 0; i < 16; i++) {
        pic_set_mask(i);
    }

    keyboard_init(vga_key_press);         // Enable keyboard, set callback

    vga_print("NeoDOS");            // Welcome message

    __asm__ __volatile__("sti");    // Enable interrupts

    while (1) {
        __asm__ __volatile__("hlt");
    }
    return;
}