#include "irq.h"
#include "pic.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"

void irq_handler(registers_t *regs) {
    uint8_t irq = regs->int_num - 32;

    if (irq == 0) { // Timer interrupt
    }

    if (irq == 1) { // Keyboard interrupt
        keyboard_handler();
    }

    pic_send_eoi(irq);
}