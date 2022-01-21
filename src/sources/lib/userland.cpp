#include <lib/userland.hpp>

#include <lib/elf.h>
#include <lib/paging.h>
#include <lib/serial.h>

#include <drivers/ustar.h>

#include <kernel.h>
#include <init/modules.h>

System::Memory::Heap System::Userland::UserHeap = System::Memory::Heap(nullptr, 0);

__cdecl void *kernel_pg_end;

void System::Userland::Init()
{
	uintptr_t uheap = (uintptr_t)kernel_pg_end;
	
	size_t memory_sz = get_full_memory_size(multiboot_data);
	//page_map_addr_sz((uintptr_t)uheap, (uintptr_t)uheap, memory_sz-uheap);
	UserHeap = Heap((void*)uheap, memory_sz-uheap, false);
}