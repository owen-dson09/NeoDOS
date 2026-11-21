#include "../drivers/vga.h"
#include "../drivers/ata.h"
#include "../kernel.h"
#include "shell.h"
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
    uint8_t text_color = getTextColor();
    setTextColor(VGA_COL_GRAY);
    vga_print("-> ");
    setTextColor(text_color);
}

static void shell_execute(const char *command) {
    while (*command == ' ') command++; // Skip spaces; later implement end trimming

    if (str_eq(command, "")) return;

    if (str_eq(command, "help")) {
        vga_puts("Available commands:");
        vga_puts("  help     - Show commands");
        vga_puts("  clear    - Clear screen");
        vga_puts("  color    - Set text color");
        vga_puts("  diskinfo - Show disk information");
        vga_puts("  echo     - Print text");
        vga_puts("  shutdown - Power off system");
        vga_puts("  version  - Show OS version");
    }
    else if (str_eq(command, "clear")) {
        vga_clear();
        setCursorPos(0, 0);
    }
    else if (str_eq(command, "color")) {
        return;
    }
    else if (str_startswith(command, "color ")) {
        const uint8_t *color_str = command + 6;
        int color = -1;

        if (str_eq(color_str, "black"))             color = VGA_COL_BLACK;
        else if (str_eq(color_str, "blue"))         color = VGA_COL_BLUE;
        else if (str_eq(color_str, "green"))        color = VGA_COL_GREEN;
        else if (str_eq(color_str, "cyan"))         color = VGA_COL_CYAN;
        else if (str_eq(color_str, "red"))          color = VGA_COL_RED;
        else if (str_eq(color_str, "magenta"))      color = VGA_COL_MAGENTA;
        else if (str_eq(color_str, "brown") 
                || str_eq(color_str, "orange"))     color = VGA_COL_BROWN;
        else if (str_eq(color_str, "light_gray") 
                || str_eq(color_str, "light_grey")) color = VGA_COL_LIGHT_GREY;
        else if (str_eq(color_str, "dark_gray") 
                || str_eq(color_str, "dark_grey") 
                || str_eq(color_str, "gray") 
                || str_eq(color_str, "grey"))       color = VGA_COL_DARK_GREY;
        else if (str_eq(color_str, "light_blue"))   color = VGA_COL_LIGHT_BLUE;
        else if (str_eq(color_str, "light_green") 
                || str_eq(color_str, "lime"))       color = VGA_COL_LIGHT_GREEN;
        else if (str_eq(color_str, "light_cyan"))   color = VGA_COL_LIGHT_CYAN;
        else if (str_eq(color_str, "light_red"))    color = VGA_COL_LIGHT_RED;
        else if (str_eq(color_str, "light_magenta") 
                || str_eq(color_str, "pink"))       color = VGA_COL_LIGHT_MAGENTA;
        else if (str_eq(color_str, "yellow"))       color = VGA_COL_YELLOW;
        else if (str_eq(color_str, "white"))        color = VGA_COL_WHITE;

        if (color != -1) {
            setTextColor((uint8_t)color);
        } else {
            vga_print("Unknown color: ");
            vga_puts(color_str);
        }
    }
    else if (str_eq(command, "diskinfo")) {
        for (uint8_t drive = 0; drive < 2; drive++) {
            const ata_device_t *device = ata_get_device(drive);

            vga_puts("----------");
            vga_print("Drive ");
            vga_print(drive == 0 ? "0" : "1");
            vga_print(": ");

            if (!device) {
                vga_puts("Not present");
                continue;
            }

            vga_puts(device->model);

            vga_print("Sectors: ");
            char sector_count_str[12];
            uint_to_str(device->sector_count, sector_count_str);
            vga_print(sector_count_str);

            vga_print(", Capacity: ");
            char capacity_str[12];
            uint_to_str(device->sector_count * 512 / 1024, capacity_str);
            vga_print(capacity_str);
            vga_puts(" KB");

            vga_print("LBA28: ");
            vga_puts(device->lba28 ? "Yes" : "No");
            vga_print("LBA48: ");
            vga_puts(device->lba48 ? "Yes" : "No");
        }
    }
    else if (str_eq(command, "echo")) {
        return;
    }
    else if (str_startswith(command, "echo ")) {
        vga_puts(command + 5);
    }
    else if (str_eq(command, "hello")) {
        vga_puts("Hello world!");
    }
    else if (str_eq(command, "shutdown")) {
        vga_puts("Shutting down system...");
        shutdown();
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

    setTextColor(VGA_COL_LIGHT_GRAY);
    setBackgroundColor(VGA_COL_BLACK);
    vga_clear();
    setCursorPos(0, 0);

    vga_puts("<< NeoDOS v0.1 >>");
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

    if (c < 32 || c > 126) return;
    if (command_length >= COMMAND_BUFFER_SIZE - 1) return;
    
    command_buffer[command_length++] = c;
    command_buffer[command_length] = '\0';

    uint8_t str[2] = {c, '\0'};
    vga_print(str);
}