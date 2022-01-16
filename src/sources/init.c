#include <stdint.h>
#include <lib/gdt.h>
#include <lib/memory.h>

static uint8_t* vidmem = (uint8_t*)0xB8000;

extern int kstack_size;
extern int kheap_size;

void* multiboot_data;

extern uint8_t kheap[];

int __primitive_print(const char *format)
{
	int i = 0, s = 0;
	while (format[s] != '\0')
	{
		vidmem[i] = format[s];
		i += 2;
		s++;
	}
	return s;
}

void clean_heap()
{
	memsetl(kheap, 0, kheap_size);
}
void clean_screen()
{
	memsetw(vidmem, 0, 80 * 25 * 2);
}

void init(void* multiboot)
{
	load_gdt();
	clean_screen();
	__primitive_print("Cleaning heap...");
	clean_heap();
	multiboot_data = multiboot;
	return;
}

void on_error(const char* message)
{
	__primitive_print(message);
}