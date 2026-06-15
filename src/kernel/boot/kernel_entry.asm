[BITS 32]
section .text.start
global _start
extern main

_start:
    call main
cli
hlt
jmp $