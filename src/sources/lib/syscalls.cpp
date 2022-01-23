#include <lib/syscalls.h>

#include <lib/memory.h>
#include <lib/paging.h>
#include <lib/heap.hpp>
#include <lib/process.hpp>
#include <lib/userland.hpp>
#include <lib/cpu.hpp>
#include <lib/serial.h>
#include <lib/gdt.h>
#include <lib/idt.hpp>

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <std/stddecl.h>

#include <kernel.h>
#include <compile_vars.h>

#define UBASE_VIRTUAL 0xC0000000

void end_handle() { while (1); }

__cdecl void syscall(regs32_t *r)
{
	using namespace System::Memory;
	using namespace System::IO;
	using namespace System::Tasking;
	
	switch (r->eax)
	{
		case 0:
			// print string
			r->eax = printf("%s", r->ebx);
			break;
		case 1:
			using namespace System::Tasking;
			if (SYS_LOG) dprintf("[SYSCALL] Closing process %d\n", ProcessManager::GetCurrentProcess()->GetPID());
			// exit system call
			page_switch_dir(get_kernel_dir());
			ProcessManager::GetCurrentProcess()->SigKill();
			r->cs = get_kernel_code_segment();
			r->ds = get_kernel_data_segment();
			r->ss = get_kernel_data_segment();
			r->eip = (uintptr_t) end_handle;
			break;
		case 2:
			{
				// userheap alloc entries
				page_switch_dir(get_kernel_dir());
				Process *proc = ProcessManager::GetCurrentProcess();
					
				uint32_t count = r->ebx;
				if (count == 0)
					return;
				else if (count > System::Userland::UserHeap.GetMaxEntries())
					return;
				r->ecx = (uint32_t) System::Userland::UserHeap.AllocEntries(count, proc->GetPID());

				if (!r->eax)
				{
					r->ebx = -1;
					return;
				}
				page_map_addr_dir_one_pg_sz(r->ecx, r->ecx+UBASE_VIRTUAL, proc->GetDir(), ENTRY_SZ*count, {present: true, rw: read_write, privilege: user, reserved_1: (0), accessed: true, dirty: true});
				r->eax = 0;
				r->ecx += UBASE_VIRTUAL;
				if (SYS_LOG) dprintf("[SYSCALL] Allocated %d entries at 0x%x\n", count, r->ecx);
				page_switch_dir(proc->GetDir());
			}
			break;
		case 3:
			{
				// vprintf
				char *fmt = (char *) r->ebx;
				va_list *args = (va_list *) r->ecx;
				r->eax = printv(fmt, *args);
			}
			break;
		default:
			r->eax = -1;
			break;
	}
}