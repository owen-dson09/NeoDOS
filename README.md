# NeoDOS

A hobby single-tasking disk operating system written from scratch in C and x86 assembly.

Currently included:
- Custom legacy BIOS bootloader
- 32-bit protected mode kernel
- VGA text mode driver w/ basic text output functions
- Keyboard driver
- Basic shell

## Features

### VGA Driver

- Clear screen
- Print text / Write text without wrapping
- Cursor positioning
- Screen scrolling
- Set Text & Background Color

### Basic Shell

- Basic commands:
```
help, clear, echo, version, color
```

## Requirements

- QEMU

## Run

```bash
qemu-system-x86_64 -hda neodos.img
```

## Planned Features

- FAT filesystem support
- System calls
- 

## License

MIT License