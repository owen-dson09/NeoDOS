# NeoDOS

A hobby disk operating system written from scratch in C and x86 assembly.

Currently included:
- Custom legacy BIOS bootloader
- 32-bit protected mode kernel
- VGA text mode driver w/ basic text output functions
- Remapped PIC and IDT

## Features

### VGA Driver

- Clear screen
- Print text / Write text without wrapping
- Cursor positioning
- Screen scrolling
- Set Text + Background Color

## Requirements

- QEMU

## Run

```bash
qemu-system-x86_64 -hda neodos.img
```

## Planned Features

- Keyboard driver
- Memory manager
- FAT filesystem support
- Command shell

## License

MIT License