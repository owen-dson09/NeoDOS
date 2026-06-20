#include "vga.h"
#include "idt.h"
#include "interrupts/pic.h"
#include "util.h"

void main() {
    idt_init();
    pic_remap();

    // Mask PIC
    for (int i = 0; i < 16; i++) {
        pic_set_mask(i);
    }
    pic_clear_mask(0); // timer irq

    vga_print("NeoDOS");

    __asm__ __volatile__("sti"); // Enable interrupts


    
    return;
}