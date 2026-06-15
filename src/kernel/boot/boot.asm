[ORG 0x7C00]
[BITS 16]

CODE_SEG equ gdt_start.code_desc - gdt_start
DATA_SEG equ gdt_start.data_desc - gdt_start
kernel_entry equ 0x90000

start:
    cli
    mov [BOOT_DRIVE], dl

    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7C00

    mov ax, 0x9000              ; Setup segment for kernel entry (0x9000:0000)
    mov es, ax

    ; Clear screen (set video mode to 80x25 text mode)
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    ; Read sector from disk into memory
    mov ax, 0x0214              ; AH = 0x02 (read sectors), AL = 20 (num sectors)
    mov cx, 0x0002              ; CH = 0 (cylinder), CL = 2 (sector)
    mov dh, 0x00                ; DH = 0 (head), DL = boot disk
    mov dl, [BOOT_DRIVE]
    xor bx, bx                  ; ES:BX = buffer address (0x9000:0000)
    int 0x13
    jc disk_retry

load_pm:
    ; Enable A20 line
    in al, 0x92
    or al, 0x02
    out 0x92, al

    ; Load PM
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:protected_mode


disk_retry:
    mov ax, 0x0214
    mov cx, 0x0002
    mov dh, 0x00
    mov dl, [BOOT_DRIVE]
    xor bx, bx
    int 0x13
    jc disk_error
    jmp load_pm
disk_error:
    mov si, msg_disk_error
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .loop
.done:
    hlt
    jmp $


gdt_start:
    ; Null segment
    .null_desc:
        dd 0, 0
    ; Code segment
    .code_desc:
        dw 0xFFFF
        dw 0x0
        db 0x0
        db 0x9A, 0xCF
        db 0x0
    ; Data segment
    .data_desc:
        dw 0xFFFF
        dw 0x0
        db 0x0
        db 0x92, 0xCF
        db 0x0
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
protected_mode:
    ; Set up segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x90000
    mov esp, ebp

    ; Jump to kernel entry point
    jmp CODE_SEG:kernel_entry

msg_disk_error db 'Failed to read from disk', 0
BOOT_DRIVE db 0

times 510 - ($ - $$) db 0
dw 0xAA55