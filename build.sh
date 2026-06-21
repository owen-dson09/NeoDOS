set -e
rm -rf build
mkdir -p build

nasm -f elf32 src/kernel/interrupts/isr.asm -o build/isr_asm.o
nasm -f elf32 src/kernel/kernel_entry.asm -o build/kernel_entry.o

CFILES=$(find src/kernel -name "*.c")

for file in $CFILES; do
    obj="build/$(basename "${file%.c}").o"
    x86_64-elf-gcc -ffreestanding -m32 \
    -fno-pie -fno-pic -fno-stack-protector \
    -fno-asynchronous-unwind-tables -fno-unwind-tables \
    -I./include -g -c "$file" -o "$obj"
done

OBJ_FILES=$(find build -name "*.o" ! -name "kernel_entry.o")

x86_64-elf-ld \
    -m elf_i386 \
    -T src/linker.ld \
    -o build/kernel.elf \
    build/kernel_entry.o \
    $OBJ_FILES

x86_64-elf-objcopy \
    -O binary \
    build/kernel.elf \
    build/kernel.bin

nasm -f bin src/kernel/boot/boot.asm -o build/boot.bin

dd if=/dev/zero of=neodos.img bs=512 count=2880 status=none
dd if=build/boot.bin of=neodos.img conv=notrunc status=none
dd if=build/kernel.bin of=neodos.img bs=512 seek=1 conv=notrunc status=none