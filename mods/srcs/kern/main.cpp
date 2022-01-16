#include <stdio.h>

extern "C" void _start()
{
	printf("Hello from kernel module\na\n");
	while(1);
}