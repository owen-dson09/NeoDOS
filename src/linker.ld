ENTRY(_start)

SECTIONS
{
    . = 0x90000;

    .text :
    {
        *(.text.start)
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }
    
    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }
}