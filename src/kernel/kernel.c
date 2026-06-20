#include "vga.h"
#include "idt.h"
#include "include/util.h"

void main() {
    idt_init();
    //__asm__ __volatile__("sti"); // Enable interrupts

    vga_print("NeoDOS Kernel Loaded!");
    return;
}