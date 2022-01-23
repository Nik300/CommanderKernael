#include <stdio.h>
#include <memory.h>

void main()
{
	printf("[Kernel] Loaded kernel module\n");
	printf("[Kernel] Test printf %d\n", 13);
	uint8_t *test = (uint8_t*)palloc(1);
	test[0x1000] = 0;
	printf("[Kernel] Test palloc 0x%x\n", test);
	while (1);
}