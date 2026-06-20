#include "irq.h"
#include "pic.h"
#include "../vga.h"

static uint32_t ticks = 0;

void irq_handler(registers_t *regs) {
    uint8_t irq = regs->int_num - 32;

    if (irq == 0) { // Timer interrupt
        ticks++;

        if (ticks % 100 == 0) {
            vga_write(".");
        }
    }

    if (irq == 1) { // Keyboard interrupt
    }

    pic_send_eoi(irq);
}