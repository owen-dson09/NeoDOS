[BITS 32]
section .text.start
global _start
extern main

_start:
    call main
    
.halt:
    hlt
    jmp .halt