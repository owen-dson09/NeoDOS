static inline void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void vga_print(const char *str);
void set_cursor_pos(int x, int y);