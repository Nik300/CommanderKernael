#include <kernel.h>

#include <std/stdio.h>
#include <std/cstring.h>
#include <std/logging.h>
#include <stddef.h>

#include <lib/memory.h>
#include <lib/idt.hpp>
#include <lib/ports.h>
#include <lib/paging.h>
#include <lib/elf.h>
#include <lib/serial.h>
#include <lib/process.hpp>

#include <init/modules.h>

#include <drivers/keyboard.h>

#include <lib/gdt.h>

extern "C" void *kheap;
extern "C" int kheap_size;
extern "C" void *kend;
__cdecl void test_umode();

extern "C" void handler(regs32_t *regs)
{
	inb(0x60);
	print("ciao\n");
	return;
}

namespace System::Kernel
{
	using System::Memory::Heap;
	using namespace System::IO;
	using namespace System::HAL;
	using namespace System::Tasking;

	Heap KernelHeap = Heap(nullptr, 0);
	KeyboardDriver *Keyboard;

	void begin()
	{
		KernelHeap = Heap(&kheap, kheap_size);
		idt32_init();
		Console::Init(stdio);
		Console::Clear();
		Serial::Init(Serial::SerialPort::COM1);
		enable_interrupts();
		page_dir_init();
		paging_enable();
		//while(1);
		Keyboard = new KeyboardDriver();
		Keyboard->Init();
		ProcessManager::Init();
	}
	void run(const char* version, const char* name)
	{
		Console::Clear();
		page_modules();

		Console::WriteLine("[%s] Booted successfully!", name);
		Console::WriteLine("[%s] Version: %s", name, version);
		Console::WriteLine("[%s] Kernel heap: 0x%x", name, KernelHeap.GetDataBuffer());
		Console::WriteLine("[%s] Kernel heap size: %dMB", name, KernelHeap.GetSize()/1024/1024);
		Console::WriteLine("[%s] Kernel heap used: %d bytes", name, KernelHeap.GetUsedSize());
		Console::WriteLine("[%s] Kernel heap free: %dMB", name, KernelHeap.GetFreeSize()/1024/1024);
		Console::WriteLine("[%s] Modules count: %d", name, multiboot_data->mods_count);
		Process *proc = ProcessManager::Create(0x0, 0x0, (ProcessEntry)0, PrivilegeLevel::Kernel);

		dyexport("dyexport", (void*)dyexport);

		dyexport("printf", (void*)printf);
		dyexport("printv", (void*)printv);
		dyexport("print", (void*)print);
		dyexport("dprintf", (void*)dprintf);

		dyexport("malloc", (void*)malloc);
		dyexport("free", (void*)free);
		
		dyexport("memset", (void*)memset);
		dyexport("memcpy", (void*)memcpy);
		dyexport("memcmp", (void*)memcmp);
		dyexport("memmove", (void*)memmov);

		dyexport("strlen", (void*)strlen);
		dyexport("strcpy", (void*)strcpy);
		dyexport("strncpy", (void*)strncpy);
		
		dyexport("strcmp", (void*)strcmp);
		dyexport("strncmp", (void*)strncmp);
		
		dyexport("strcat", (void*)strcat);
		dyexport("strncat", (void*)strncat);
		
		dyexport("strchr", (void*)strchr);
		dyexport("strrchr", (void*)strrchr);

		dyexport("strstr", (void*)strstr);
		dyexport("strrstr", (void*)strstr);
		
		dyexport("strtok", (void*)strtok);
		dyexport("strrtok", (void*)strtok_r);
		
		dyexport("strnum", (void*)strnum);
		dyexport("strdec", (void*)strdec);
		dyexport("strhex", (void*)strhex);
		dyexport("strbin", (void*)strbin);
		dyexport("stroct", (void*)stroct);
		
		dyexport("numstr", (void*)numstr);
		dyexport("decstr", (void*)decstr);
		dyexport("hexstr", (void*)hexstr);
		dyexport("binstr", (void*)binstr);
		dyexport("octstr", (void*)octstr);

		dyexport("numstr_buf", (void*)numstr_buf);

		dyexport("strrev", (void*)strrev);

		dyexport("regirq", (void*)register_int_handler);
		
		dyexport("elf_load", (void*)elf32_load);

		void *test_mod = get_module(0);
		elf32_load(test_mod, get_module_size(0), 3);

		//asm ("int $0x2");

		Keyboard->Activate();
		while (1);
	}
}