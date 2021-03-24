clear
nasm -f elf32 Headers/core/cpu/interrupts/interrupt.asm -o ../compiled/kernel/interrupt.o
nasm -f elf32 Headers/core/cpu/interrupts/realmode.asm -o ../compiled/kernel/realmode.o
nasm -f elf32 'Sources/entry/gdt.asm' -o ../compiled/kernel/gdt.o
as --32 Headers/core/cpu/interrupts/stack_switcher.s -o ../compiled/kernel/stack_switcher.o
as --32 'Sources/entry/kernel_entry.asm' -o ../compiled/kernel/kernel_entry.o
gcc -ffreestanding -m32 -c Headers/drivers/mouse.c -o ../compiled/kernel/mouse.o -fno-pie -I .
gcc -ffreestanding -m32 -c Headers/drivers/serial.c -o ../compiled/kernel/serial.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/drivers/ata.c -o ../compiled/kernel/ata.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/core/cpu/interrupts/idt.c -o ../compiled/kernel/idt.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/core/cpu/interrupts/isr.c -o ../compiled/kernel/isr.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/core/cpu/memory/mapping.c -o ../compiled/kernel/mapping.o -I. -fno-pie
gcc -ffreestanding -c kernel.cpp -o ../compiled/kernel/kernel.o -m32 -I. -fno-threadsafe-statics -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fno-pie
gcc -ffreestanding -c Sources/utility/ports.cpp -o ../compiled/kernel/ports_cpp.o -m32 -I. -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fno-pie
gcc -ffreestanding -m32 -c Headers/utility/util.c -o ../compiled/kernel/util.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/utility/ports.c -o ../compiled/kernel/ports.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/utility/kalloc.c -o ../compiled/kernel/kalloc.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/utility/frame.c -o ../compiled/kernel/frame.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/utility/paging.c -o ../compiled/kernel/paging.o -I. -fno-pie
gcc -ffreestanding -m32 -c Headers/stdio/stdio.c -o ../compiled/kernel/stdio.o -I. -fno-pie
ld -melf_i386 -T linker.ld ../compiled/kernel/kernel_entry.o\
			               ../compiled/kernel/gdt.o\
                           ../compiled/kernel/frame.o\
                           ../compiled/kernel/paging.o\
                           ../compiled/kernel/stack_switcher.o\
                           ../compiled/kernel/kalloc.o\
                           ../compiled/kernel/ports_cpp.o\
                           ../compiled/kernel/kernel.o\
                           ../compiled/kernel/realmode.o\
                           ../compiled/kernel/stdio.o\
                           ../compiled/kernel/util.o\
                           ../compiled/kernel/serial.o\
                           ../compiled/kernel/ports.o\
                           ../compiled/kernel/interrupt.o\
                           ../compiled/kernel/idt.o\
                           ../compiled/kernel/isr.o\
                           ../compiled/kernel/mouse.o\
                           ../compiled/kernel/mapping.o\
                           ../compiled/kernel/ata.o\
                           -o ../compiled/bootable/bootable.elf
mkdir ../compiled/bootable/iso
mkdir ../compiled/bootable/iso/boot
mkdir ../compiled/bootable/iso/boot/grub
cp ../compiled/bootable/bootable.elf ../compiled/bootable/iso/boot/commanderkernael.bin
echo "set color_normal=blue/green"       >	../compiled/bootable/iso/boot/grub/grub.cfg
echo 'set timeout=5'                     >> ../compiled/bootable/iso/boot/grub/grub.cfg
echo 'set default=0'                     >> ../compiled/bootable/iso/boot/grub/grub.cfg
echo ''                                  >> ../compiled/bootable/iso/boot/grub/grub.cfg
echo 'menuentry "CommanderKernael" {' >> ../compiled/bootable/iso/boot/grub/grub.cfg
echo '  multiboot /boot/commanderkernael.bin'    >> ../compiled/bootable/iso/boot/grub/grub.cfg
echo '  boot'                            >> ../compiled/bootable/iso/boot/grub/grub.cfg
echo '}'                                 >> ../compiled/bootable/iso/boot/grub/grub.cfg
grub-mkrescue --output=../compiled/bootable/commanderkernael.iso ../compiled/bootable/iso
rm -rf ../compiled/bootable/iso
