#ifndef VGA_H
#define VGA_H

void set_cursor_pos(int x, int y);
unsigned short get_cursor_pos(void);
void vga_clear();
void vga_clear_line();
void vga_scroll(int y);
void vga_putc(char c, unsigned char color, int x, int y);
void vga_write(const char *str);
void vga_print(const char *str);

#endif