#include <kernel.h>

#include <std/stdio.h>
#include <std/cstring.h>
#include <std/logging.h>
#include <std/stddecl.h>
#include <stddef.h>

#include <lib/memory.h>
#include <lib/idt.hpp>
#include <lib/ports.h>
#include <lib/paging.h>
#include <lib/elf.h>
#include <lib/cpu.hpp>
#include <lib/serial.h>
#include <lib/process.hpp>
#include <lib/gdt.h>
#include <lib/userland.hpp>

#include <init/modules.h>
#include <drivers/keyboard.h>
#include <drivers/ustar.h>

__cdecl void *kheap;
__cdecl int kheap_size;
__cdecl void *kend;

namespace System::Kernel
{
	using System::Memory::Heap;
	using namespace System::IO;
	using namespace System::HAL;
	using namespace System::Tasking;

	Heap KernelHeap = Heap(nullptr, 0);
	KeyboardDriver *Keyboard;
	Process *KernelProc;

	void begin()
	{
		KernelHeap = Heap(&kheap, kheap_size);
		idt32_init();
		Console::Init(stdio);
		Console::Clear();
		Serial::Init(Serial::SerialPort::COM1);
		enable_interrupts();
		ramdisk = (uint8_t*)get_module(0);
		tar_calc_size();
		page_dir_init();
		paging_enable();
		page_map_addr_sz((uintptr_t)ramdisk, (uintptr_t)ramdisk, ramdisk_size);
		Keyboard = new KeyboardDriver();
		Keyboard->Init();
		ProcessManager::Init();
		ProcessManager::ToggleLog();
	}
	void run(const char* version, const char* name, const char* OSName)
	{
		Console::Clear();

		Console::WriteLine(ConsoleColor::Yellow, "[%s] %s Booted successfully!", name, OSName);
		Console::WriteLine("[%s] Version: %s", name, version);
		Console::WriteLine("[%s] Kernel heap: 0x%x", name, KernelHeap.GetDataBuffer());
		Console::WriteLine("[%s] Kernel heap size: %dMB", name, KernelHeap.GetSize()/1024/1024);
		Console::WriteLine("[%s] Kernel heap used: %d bytes", name, KernelHeap.GetUsedSize());
		Console::WriteLine("[%s] Kernel heap free: %dMB", name, KernelHeap.GetFreeSize()/1024/1024);
		Console::WriteLine("[%s] Modules count: %d", name, multiboot_data->mods_count);
		Console::WriteLine("[%s] Kernel end at: %x", name, &kend);
		Console::WriteLine("[%s] Ramdisk at: %x", name, ramdisk);
		Console::WriteLine("[%s] Installed Memory: %dMB", name, get_full_memory_size(multiboot_data)/1024/1024);
		
		System::Userland::Init();
		
		Console::WriteLine("[%s] Userland heap: 0x%x", name, System::Userland::UserHeap.GetDataBuffer());
		Console::WriteLine("[%s] Userland heap size: %dMB", name, System::Userland::UserHeap.GetSize()/1024/1024);
		Console::WriteLine("[%s] Userland heap used: %d bytes", name, System::Userland::UserHeap.GetUsedSize());
		Console::WriteLine("[%s] Userland heap free: %dMB", name, System::Userland::UserHeap.GetFreeSize()/1024/1024);

		//ProcessManager::Create(0, 0, [](char *argv[], int argc) { while (1); }, PrivilegeLevel::Kernel);

		//page_map_addr_sz((uintptr_t)get_module(0), (uintptr_t)get_module(0), get_module_size(0), { present: true, rw: read_write, privilege: supervisor, 0, accessed: false, dirty: true });
		//elf32_load(get_module(0), get_module_size(0), 3);

		//uint8_t *test = tar_fopen("./modules/test.mod");
		//elf32_load(test, tar_ftell("./modules/test.mod"), 3);

		Keyboard->Activate();
		while (1);
	}
}