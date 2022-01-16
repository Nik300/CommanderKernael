#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <dylink.h>
#include <cstring.h>
#include <kernel.h>

extern "C" void _start()
{
	char *test = "Hello from userpace module!\n";
	asm ("movl %0, %%ebx" : : "r"(test));
	asm ("movl $0, %eax;int $0x30");
	while (1);
}