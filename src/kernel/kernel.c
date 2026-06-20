#include "vga.h"
#include "include/util.h"

void main() {
    vga_print("NeoDOS Kernel Loaded!");
    set_text_color(VGA_COL_YELLOW);
    vga_print("This should be yellow.");
    set_background_color(VGA_COL_BLUE);
    set_text_color(VGA_COL_WHITE);
    vga_print("This should be white on blue.");
    return;
}