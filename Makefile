CXX=i686-elf-g++
CC=i686-elf-gcc
LD=i686-elf-ld
NAME=Commander
CXXFLAGS=-Isrc/inc -fno-use-cxa-atexit -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -w -Wno-narrowing -Wno-sign-compare -Wno-type-limits -Wno-unused-parameter -Wno-missing-field-initializers
CFLAGS=-Isrc/inc -fno-use-cxa-atexit -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -w -Wno-narrowing -Wno-sign-compare -Wno-type-limits -Wno-unused-parameter -Wno-missing-field-initializers
NASM=nasm
ASFLAGS=-felf32
CXX_SOURCES=$(shell find src -name '*.cpp' -not -path "initrd/*")
C_SOURCES=$(shell find src -name '*.c' -not -path "initrd/*")
LINK_SOURCES=$(shell find src -name '*.o' -not -path "initrd/*")
MODULES=$(shell find ./mods -name '*.mod' -not -path "initrd/*")
MODULES_COMPILE=$(shell find ./mods -name '*.sh' -not -path "initrd/*")
MODULES_OUTPUT=$(MODULES_COMPILE:.sh=.mod)
CPP_FILES_OUT = $(CXX_SOURCES:.cpp=.cpp.o)
C_FILES_OUT = $(C_SOURCES:.c=.c.o)


all: as $(MODULES_OUTPUT) $(C_FILES_OUT) $(CPP_FILES_OUT) initrd link grub run-kvm
bochs: as $(MODULES_OUTPUT) $(C_FILES_OUT) $(CPP_FILES_OUT) initrd link grub run-bochs

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
	echo "test" >> isoroot/mods/test_txt.mod
	cp kernel.bin isoroot/boot
	cp src/grub/grub.cfg isoroot/boot/grub
	grub-mkrescue -o $(NAME).iso isoroot -V "Commander"

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.mod: %.sh
	bash $<
	
run:
	qemu-system-i386 -cdrom $(NAME).iso -serial stdio -vga std -no-reboot -no-shutdown -m 1G
run-kvm:
	qemu-system-i386 -cdrom $(NAME).iso -serial stdio -vga std -no-reboot -no-shutdown -m 1G -enable-kvm -cpu host
run-bochs:
	/usr/local/bin/bochs -q -f bochsrc.txt

initrd:
	mkdir -p initrd
	cp -r $(MODULES_OUTPUT) initrd
	tar czf  initrd.img -C initrd/ ./ --format=ustar
	mkdir -p isoroot/initrd
	mv initrd.img isoroot/initrd
	rm -Rf initrd
