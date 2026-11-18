#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "drivers/ata.h"
#include "idt.h"
#include "interrupts/pic.h"
#include "shell/shell.h"
#include "util.h"

#define KEYBOARD_BUFFER_SIZE 256

static volatile uint8_t keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile uint16_t keyboard_buffer_head = 0;
static volatile uint16_t keyboard_buffer_tail = 0;

static void kernel_key_press(char c) {
    // Add character to keyboard buffer
    uint16_t next = (keyboard_buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
    if (next == keyboard_buffer_tail) return; // Buffer full = discard character
    keyboard_buffer[keyboard_buffer_head] = c;
    keyboard_buffer_head = next;
}

void main() {
    idt_init();
    pic_remap();

    // Mask PIC (ignore hardware interrupts)
    for (int i = 0; i < 16; i++) {
        pic_set_mask(i);
    }

    // VGA is already cleared from bootloader, but clear again to be sure
    vga_clear();
    setCursorPos(0, 0);

    shell_init();
    keyboard_init(kernel_key_press); // Enable keyboard, set callback

    while (1) {
        __asm__ __volatile__("cli");
        
        // Check if there is a character in the keyboard buffer
        if (keyboard_buffer_head != keyboard_buffer_tail) {
            char c = keyboard_buffer[keyboard_buffer_tail];
            keyboard_buffer_tail = (keyboard_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;

            // Handle the character in the shell
            shell_handle_char(c);
        }

        __asm__ __volatile__("sti; hlt");
    }
    return;
}

// Shutdown for QEMU, temporary
__attribute__((noreturn)) void shutdown() {
    __asm__ __volatile__("cli");

    outw(0x604, 0x2000); // QEMU shutdown
    outw(0xB004, 0x2000); // Bochs shutdown

    while (1) {
        __asm__ __volatile__("hlt");
    }
}