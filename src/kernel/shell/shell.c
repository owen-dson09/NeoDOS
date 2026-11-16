#include "shell.h"
#include "../drivers/vga.h"
#include "util.h"

#define COMMAND_BUFFER_SIZE 256

static uint8_t command_buffer[COMMAND_BUFFER_SIZE];
static uint16_t command_length = 0;

//// Helper functions ////
    static int str_eq(const char *a, const char *b) {
        while (*a && *b) {
            if (*a != *b) return 0;
            a++;
            b++;
        }
        return !*a && !*b;
    }

    static int str_startswith(const char *str, const char *prefix) {
        while (*prefix) {
            if (*str != *prefix) return 0;
            str++;
            prefix++;
        }
        return 1;
    }
//// END Helper functions ////

static void shell_prompt(void) {
    setTextColor(VGA_COL_GRAY);
    vga_print("-> ");
    setTextColor(VGA_COL_LIGHT_GRAY);
}

static void shell_execute(const char *command) {
    if (str_eq(command, "")) return;

    if (str_eq(command, "help")) {
        vga_puts("Available commands:");
        vga_puts("  help    - Show commands");
        vga_puts("  clear   - Clear screen");
        vga_puts("  echo    - Print text");
        vga_puts("  version - Show OS version");
    }
    else if (str_eq(command, "clear")) {
        vga_clear();
        setCursorPos(0, 0);
    }
    else if (str_eq(command, "echo")) {
        vga_puts("");
    }
    else if (str_startswith(command, "echo ")) {
        vga_puts(command + 5);
    }
    else if (str_eq(command, "version")) {
        vga_puts("NeoDOS v0.1");
    }
    else {
        vga_print("Unknown command: ");
        vga_puts(command);
    }
}

void shell_init(void) {
    command_length = 0;
    command_buffer[0] = '\0';

    vga_puts("-- NeoDOS v0.1 --");
    shell_prompt();
}

void shell_handle_char(char c) {
    // Enter
    if (c == '\n' || c == '\r') {
        command_buffer[command_length] = '\0';

        vga_puts("");
        shell_execute(command_buffer);

        command_length = 0;
        command_buffer[0] = '\0';

        shell_prompt();
        return;
    }

    // Backspace
    if (c == '\b') {
        if (command_length > 0) {
            command_length--;
            command_buffer[command_length] = '\0';
            vga_backspace();
        }
        return;
    }

    // Ignore non-printable characters
    if (c < 32 || c > 126) 
        return;

    // Prevent overflow
    if (command_length >= COMMAND_BUFFER_SIZE - 1)
        return;
    
    command_buffer[command_length++] = c;
    command_buffer[command_length] = '\0';

    uint8_t str[2] = {c, '\0'};
    vga_print(str);
}