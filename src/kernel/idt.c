#include "idt.h"
#include "vga.h"
#include "include/util.h"

extern void isr_default_stub(void);

static idt_entry_t idt[256];
static idt_ptr_t idtr;

static void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low     = handler & 0xFFFF;
    idt[num].selector       = selector;
    idt[num].zero           = 0;
    idt[num].type_attr      = flags;
    idt[num].offset_high    = (handler >> 16) & 0xFFFF;
}

void isr_default_handler(void) {
    vga_print("Unhandled interrupt");
}

void idt_init(void) {
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)isr_default_stub, 0x08, 0x8E);
    }

    __asm__ __volatile__("lidt %0" : : "m"(idtr));
}