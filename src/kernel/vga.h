#ifndef VGA_H
#define VGA_H

#define VGA_COL_BLACK 0
#define VGA_COL_BLUE 1
#define VGA_COL_GREEN 2
#define VGA_COL_CYAN 3
#define VGA_COL_RED 4
#define VGA_COL_MAGENTA 5
#define VGA_COL_BROWN 6
#define VGA_COL_LIGHT_GREY 7
#define VGA_COL_DARK_GREY 8
#define VGA_COL_LIGHT_BLUE 9
#define VGA_COL_LIGHT_GREEN 10
#define VGA_COL_LIGHT_CYAN 11
#define VGA_COL_LIGHT_RED 12
#define VGA_COL_LIGHT_MAGENTA 13
#define VGA_COL_YELLOW 14
#define VGA_COL_WHITE 15

void set_cursor_pos(int x, int y);
unsigned short get_cursor_pos(void);
void set_text_color(unsigned char color);
void set_background_color(unsigned char color);
void vga_clear();
void vga_clear_line();
void vga_scroll(int y);
void vga_putc(char c, unsigned char color, int x, int y);
void vga_write(const char *str);
void vga_print(const char *str);

#endif