CXX=i686-elf-g++
CC=i686-elf-gcc
LD=i686-elf-ld
NAME=StormOS
CXXFLAGS=-Isrc/inc -fno-use-cxa-atexit -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -w -Wno-narrowing -Wno-sign-compare -Wno-type-limits -Wno-unused-parameter -Wno-missing-field-initializers
CFLAGS=-Isrc/inc -fno-use-cxa-atexit -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -w -Wno-narrowing -Wno-sign-compare -Wno-type-limits -Wno-unused-parameter -Wno-missing-field-initializers
NASM=nasm
ASFLAGS=-felf32
CXX_SOURCES=$(shell find src -name '*.cpp')
C_SOURCES=$(shell find src -name '*.c')
LINK_SOURCES=$(shell find src -name '*.o')
MODULES=$(shell find ./mods -name '*.mod')
MODULES_COMPILE=$(shell find ./mods -name '*.sh')
MODULES_OUTPUT=$(MODULES_COMPILE:.sh=.mod)
CPP_FILES_OUT = $(CXX_SOURCES:.cpp=.o)
C_FILES_OUT = $(C_SOURCES:.c=.o)


all: as $(MODULES_OUTPUT) $(C_FILES_OUT) $(CPP_FILES_OUT) link grub clean_objects run-kvm
bochs: as $(MODULES_OUTPUT) $(C_FILES_OUT) $(CPP_FILES_OUT) link grub clean_objects run-bochs

as:
	i686-elf-as --32 'src/asm/boot.s' -o 'src/asm/boot.a'
	$(NASM) $(ASFLAGS) 'src/asm/idt_handlers.asm' -o 'src/asm/idt_handlers.o'
	$(NASM) $(ASFLAGS) 'src/asm/gdt_helpers.asm' -o 'src/asm/gdt_helpers.o'
link:
	$(LD) -Tlinker.ld -o 'kernel.bin' -nostdlib 'src/asm/boot.a' $(LINK_SOURCES)    

clean:
	-rm -Rf $(shell find . -name '*.o') $(shell find . -name '*.bin') $(shell find . -name '*.iso') $(MODULES_OUTPUT)

clean_objects:
	-rm -Rf $(shell find . -name '*.o') $(shell find . -name '*.a') $(MODULES_OUTPUT)

grub:
	mkdir -p isoroot/boot/grub
	mkdir -p isoroot/mods
	cp -r $(MODULES) isoroot/mods
	cp kernel.bin isoroot/boot
	cp src/grub/grub.cfg isoroot/boot/grub
	grub-mkrescue -o $(NAME).iso isoroot -V "Commander"

%.mod: %.sh
	bash $<
	
run:
	qemu-system-i386 -cdrom $(NAME).iso -serial stdio -vga std -no-reboot -no-shutdown -m 1G
run-kvm:
	qemu-system-i386 -cdrom $(NAME).iso -serial stdio -vga std -no-reboot -no-shutdown -m 1G -enable-kvm
run-bochs:
	/usr/local/bin/bochs -q -f bochsrc.txt