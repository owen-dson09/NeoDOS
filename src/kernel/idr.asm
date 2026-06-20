[BITS 32]

global isr_default_stub
extern isr_default_handler

isr_default_stub:
    pusha
    call isr_default_handler
    popa
    iret