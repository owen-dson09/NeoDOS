#include "isr.h"
#include "pic.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"

static const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

void isr_handler(registers_t *regs) {
    vga_print("CPU Exception: ");

    if (regs->int_num < 32) {
        vga_print(exception_messages[regs->int_num]);
    }

    while (1) {
        __asm__ __volatile__("cli; hlt"); // Temporary
    }
}

void irq_handler(registers_t *regs) {
    uint8_t irq = regs->int_num - 32;

    if (irq == 0) { // Timer interrupt
    }

    if (irq == 1) { // Keyboard interrupt
        keyboard_handler();
    }

    pic_send_eoi(irq);
}