#include <kernel.h>
#include <stdio.h>

void main()
{
	asm ("xchg %bx, %bx");
	print("Hello, world!\n");
	exit();
	while (1);
}