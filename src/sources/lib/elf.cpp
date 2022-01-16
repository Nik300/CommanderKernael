#include <kernel.h>

#include <lib/elf.h>
#include <lib/memory.h>
#include <lib/paging.h>
#include <lib/serial.h>

#include <lib/process.hpp>

#include <std/cstring.h>
#include <std/stdio.h>

__cdecl uint8_t *current_module = nullptr;

__cdecl bool elf32_validate_header(void *data)
{
	elf32_header_t *header = (elf32_header_t*)data;

	static const char magic[] = { 0x7F, 'E', 'L', 'F' };

	return memcmpl(header->identifier.magic, magic, sizeof(magic));
}
__cdecl uint32_t elf32_load(void *data, size_t size, uint8_t privilege)
{
	using namespace System::Tasking;

	dprintf("Loading ELF32 module...\n");

	if (!elf32_validate_header(data))
	{
		dprintf("[%s] Invalid or unrecognized elf header.\n", "ELF");
		return 0;
	}

	elf32_header *header = (elf32_header*)data;
	elf32_prog_hdr *prog_hdr = (elf32_prog_hdr*)((uintptr_t)data + (uintptr_t)header->phdr_table);
	Process *proc = ProcessManager::Create(0, 0, (ProcessEntry)header->entry, (PrivilegeLevel)privilege);

	dprintf("[%s] Loading ELF module...\n", "ELF");
	dprintf("[%s] 	ELF entry point: 0x%x\n", "ELF", header->entry);
	dprintf("[%s] 	ELF program header table: 0x%x\n", "ELF", header->phdr_table);
	dprintf("[%s] 		ELF program header count: %d\n", "ELF", header->phdr_count);
	dprintf("[%s] 	ELF section header table: 0x%x\n", "ELF", header->shdr_table);
	dprintf("[%s] 		ELF section header count: %d\n", "ELF", header->shdr_count);

	uint8_t *prog_data = (uint8_t*)calloc(size + 0x2000);
	current_module = prog_data;
	
	for (uintptr_t i = 0; i < header->phdr_count; i++)
	{
		if (prog_hdr[i].type == 1)
		{
			dprintf("[%s] Loading program header %d...\n", "ELF", i);
			dprintf("[%s] 	Type: %d\n", "ELF", prog_hdr[i].type);
			dprintf("[%s] 	Offset: 0x%x\n", "ELF", prog_hdr[i].offset);
			dprintf("[%s] 	VirtAddr: 0x%x\n", "ELF", prog_hdr[i].vaddr);
			dprintf("[%s] 	PhysAddr: 0x%x\n", "ELF", prog_hdr[i].paddr);
			dprintf("[%s] 	FileSize: %d\n", "ELF", prog_hdr[i].filesz);
			dprintf("[%s] 	MemSize: %d\n", "ELF", prog_hdr[i].memsz);
			dprintf("[%s] 	Flags: %d\n", "ELF", prog_hdr[i].flags);
			dprintf("[%s] 	Align: %d\n", "ELF", prog_hdr[i].align);

			page_map_addr_sz((uint32_t)mem_align(prog_data), prog_hdr[i].vaddr, prog_hdr[i].filesz+0x2000, {present: true, rw: read_write, privilege: user, reserved_1: (0), accessed: false, dirty: false});
			page_map_addr_dir_sz((uint32_t)mem_align(prog_data), prog_hdr[i].vaddr, proc->GetDir(), size+0x2000, {present: true, rw: read_write, privilege: user, reserved_1: (0), accessed: false, dirty: false});

			memcpy((void*)mem_align(prog_data), (uint8_t*)data+prog_hdr[i].offset, size + 0x1000);

			page_unmap_addr_sz(prog_hdr[i].vaddr, size+0x2000);

			prog_data += mem_align((void*)prog_hdr[i].memsz);
			dprintf("[%s] 	Program header %d loaded.\n", "ELF", i);
		}
	}

	//while(1);
	proc->SigRun();

	return (uint32_t)proc;
}
__cdecl void elf32_unload()
{
	if (current_module)
	{
		free(current_module);
		current_module = nullptr;
	}
}