#include "pic.h"
#include "util.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI      0x20

void pic_remap(void) {
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // ICW1
    outb(PIC1_COMMAND, 0x11); // 0x10 (Initialize PIC) | 0x01 (ICW4)
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    // ICW2
    outb(PIC1_DATA, 0x20); // Master PIC -> 0x20-0x27
    io_wait();
    outb(PIC2_DATA, 0x28); // Slave PIC -> 0x28-0x2F
    io_wait();

    // ICW3
    outb(PIC1_DATA, 0x04); // Tell master that slave is at IRQ2
    io_wait();
    outb(PIC2_DATA, 0x02); // Tell slave its identity
    io_wait();

    // ICW4
    outb(PIC1_DATA, 0x01); // 0x01 = 8086/8088 mode
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_set_mask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }

    value = inb(port) | (1 << irq);
    outb(port, value);
}

void pic_clear_mask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }

    value = inb(port) & ~(1 << irq);
    outb(port, value);
}