static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void vclear();
void set_cursor_pos(int x, int y);
unsigned short get_cursor_pos(void);
void vputc(char c, unsigned char color, int x, int y);
void vwrite(const char *str);
void vprint(const char *str);