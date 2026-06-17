# NeoDOS

A hobby disk operating system written from scratch in C and x86 assembly.

Currently included:
- Custom legacy BIOS bootloader
- 32-bit protected mode kernel
- VGA text mode driver
- Basic text output functions

## Features

### Boot Process

- Legacy BIOS boot sector
- Loads kernel from disk
- Enables A20 line
- Switches to protected mode
- Jumps to kernel entry point

### VGA Driver

- Clear screen
- Print text
- Write text without wrapping
- Cursor positioning
- Screen scrolling

## Requirements

- NASM
- x86_64-elf-gcc
- x86_64-elf-ld
- QEMU

# Build

```bash
chmod +x ./build.sh
./build.sh
```

This creates:

```
neodos.img
```

## Run

```bash
qemu-system-x86_64 -hda neodos.img
```

## Planned Features

- Exception handlers
- PIC remapping
- Keyboard driver
- Memory manager
- FAT filesystem support
- Command shell

## License

MIT License