extern "C" void _start()
{
	asm ("mov $3, %ebx;mov $2, %eax;int $0x30");
	asm ("mov $1, %eax;int $0x30");
	while (1);

	//asm ("movl $3, %ebx;movl $2, %eax;int $0x30");

	while(1);

	// store address in ecx in variable 'addr'
	//char *addr;
	//asm ("movl %%ecx, %0" : "=r" (addr));
}