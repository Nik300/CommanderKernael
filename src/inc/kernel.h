#pragma once
#include <lib/multiboot.h>
#ifdef __cplusplus
#include <lib/heap.hpp>

namespace System::Kernel
{
	extern System::Memory::Heap KernelHeap;

	void begin();
	void run(const char* version, const char* name, const char* OSName);
}

extern "C" {
#endif

extern void main();
extern void init(void* multiboot);
extern multiboot_info_t *multiboot_data;

#ifdef __cplusplus
}
#endif