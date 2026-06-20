[BITS 32]
section .text
global _start
extern main

_start:
    call main
    
.halt:
    hlt
    jmp .halt