#include <kernel.h>

extern "C" void main()
{
	System::Kernel::begin();
	for (;;) System::Kernel::run("Alpha 0.0.1", "CommanderKernael", "AerOS");
}