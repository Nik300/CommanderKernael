#include <kernel.h>
#include <stdio.h>

void main()
{
	asm ("xchg %bx, %bx");
	int i = print("Hello, world\n");
	if (i == 13) print("test\n");
	exit();
	print("Hello, world 2!\n");
	while (1);
}