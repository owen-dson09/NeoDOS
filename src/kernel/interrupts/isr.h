#ifndef ISR_H
#define ISR_H

#include "../include/util.h"

typedef struct registers {
    uint32_t ds;

    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    uint32_t int_no;
    uint32_t err_code;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} registers_t;

void isr_handler(registers_t *regs);

#endif