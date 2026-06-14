#include "vga.h"

void main() {
    vga_print("NeoDOS Kernel Loaded!");
    set_cursor_pos(0, 1);
    return;
}