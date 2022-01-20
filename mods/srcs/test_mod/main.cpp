#include <kernel.h>
#include <stdio.h>

extern "C" void _start()
{
	asm ("xchg %bx, %bx");
	print("Hello, world!\n");
	while (1);
}