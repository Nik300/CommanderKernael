nasm -f elf32 Headers/core/cpu/interrupts/interrupt.asm -o ../compiled/kernel/interrupt.o
nasm -f elf32 'Sources/entry/kernel_entry.asm' -o ../compiled/kernel/kernel_entry.o
gcc -ffreestanding -m32 -c Headers/core/cpu/interrupts/idt.c -o ../compiled/kernel/idt.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/core/cpu/interrupts/isr.c -o ../compiled/kernel/isr.o -I. -fno-pie
gcc -ffreestanding -c kernel.cpp -o ../compiled/kernel/kernel.o -m32 -I. -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fno-pie
gcc -ffreestanding -c Sources/utility/ports.cpp -o ../compiled/kernel/ports_cpp.o -m32 -I. -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fno-pie
gcc -ffreestanding -m32 -c Headers/utility/util.c -o ../compiled/kernel/util.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/utility/ports.c -o ../compiled/kernel/ports.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/stdio/stdio.c -o ../compiled/kernel/stdio.o -I. -fno-pie
ld -melf_i386 --oformat binary -Ttext 0x1000 ../compiled/kernel/kernel_entry.o ../compiled/kernel/ports_cpp.o ../compiled/kernel/kernel.o ../compiled/kernel/stdio.o ../compiled/kernel/util.o ../compiled/kernel/ports.o ../compiled/kernel/interrupt.o ../compiled/kernel/idt.o ../compiled/kernel/isr.o -o ../compiled/bootable/bootable.row
cat ../compiled/bootloader/bootloader.bin ../compiled/bootable/bootable.row > ../compiled/bootable/bootable.row.img
dd if=../compiled/bootable/bootable.row.img of=../compiled/bootable/bootable.img conv=notrunc
qemu-system-i386 -fda ../compiled/bootable/bootable.img -vga vmware
