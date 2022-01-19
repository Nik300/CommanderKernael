#include <lib/process.hpp>
#include <lib/idt.hpp>
#include <lib/gdt.h>
#include <lib/ports.h>
#include <lib/memory.h>
#include <lib/userland.hpp>

#include <std/stdio.h>
#include <lib/serial.h>
#include <init/modules.h>

#include <kernel.h>

#define PROC_ENTRIES_COUNT (sizeof(Process)/ENTRY_SZ)+1

__cdecl uintptr_t procs_addr;
__cdecl void *kend;

namespace System::Tasking
{
	Process::Process(uint32_t vaddr, uint32_t paddr, ProcessEntry entry, PrivilegeLevel privilege)
	{

		uint32_t cs;
		uint32_t ds;
		
		switch (privilege)
		{
		case PrivilegeLevel::Kernel:
			cs = get_kernel_code_segment();
			ds = get_kernel_data_segment();
			break;
		case PrivilegeLevel::Module:
			cs = get_module_code_segment();
			ds = get_module_data_segment();
			break;
		case PrivilegeLevel::Driver:
			cs = get_driver_code_segment();
			ds = get_driver_data_segment();
			break;
		case PrivilegeLevel::User:
			cs = get_user_code_segment();
			ds = get_user_data_segment();
			break;
		default:
			break;
		}

		regs = (regs32_t) {
			.ds = ds,

			.edi = 0,
			.esi = 0,
			.ebp = 0,
			.esp = (uint32_t)&stack[sizeof(stack)],
			.ebx = 0,
			.edx = 0,
			.ecx = 0,
			.eax = 0,

			.identifier =  {
				.interrupt_number = 0
			},

			.eip = (uint32_t)entry,
			.cs  = cs,
			.eflags = 0x202,
			.useresp = 0,
			.ss = ds
		};

		this->is_alive = true;
		this->is_running = false;
		this->is_waiting = false;
		this->is_sleeping = false;

		this->has_crashed = false;
		this->has_exited = false;

		this->pid = 0;
		this->virt_addr = vaddr;
		this->phys_addr = paddr;
		this->entry = entry;
		this->privilege = privilege;
	}
	Process::~Process()
	{

	}
	void Process::Init(uint32_t vaddr, uint32_t paddr, ProcessEntry entry, PrivilegeLevel privilege)
	{
		this->virt_addr = vaddr;
		this->phys_addr = paddr;
		this->entry = entry;
		this->privilege = privilege;

		uint32_t cs;
		uint32_t ds;

		switch (privilege)
		{
		case PrivilegeLevel::Kernel:
			cs = get_kernel_code_segment();
			ds = get_kernel_data_segment();
			break;
		case PrivilegeLevel::Module:
			cs = get_module_code_segment();
			ds = get_module_data_segment();
			break;
		case PrivilegeLevel::Driver:
			cs = get_driver_code_segment();
			ds = get_driver_data_segment();
			break;
		case PrivilegeLevel::User:
			cs = get_user_code_segment();
			ds = get_user_data_segment();
			break;
		default:
			break;
		}

		regs = (regs32_t) {
			.ds = ds,

			.edi = 0,
			.esi = 0,
			.ebp = 0,
			.esp = (uint32_t)&stack[sizeof(stack)],
			.ebx = 0,
			.edx = 0,
			.ecx = 0,
			.eax = 0,

			.identifier =  {
				.interrupt_number = 0
			},

			.eip = (uint32_t)entry,
			.cs  = cs,
			.eflags = 0x202,
			.useresp = 0,
			.ss = ds
		};

		this->is_alive = false;
		this->is_running = false;
		this->is_waiting = false;
		this->is_sleeping = false;

		this->has_crashed = false;
		this->has_exited = false;
		
		this->dir = (page_dir_t*)mem_align((void*)pad);

		this->pid = 0;
	}
	void Process::SigExit()
	{
		this->is_alive = false;
		this->is_running = false;
		this->is_waiting = false;
		this->is_sleeping = false;
		this->has_crashed = false;
		this->has_exited = true;
	}
	void Process::SigCrash()
	{
		this->is_alive = false;
		this->is_running = false;
		this->is_waiting = false;
		this->is_sleeping = false;
		this->has_crashed = true;
		this->has_exited = false;
	}
	void Process::SigWait()
	{
		this->is_alive = true;
		this->is_running = false;
		this->is_waiting = true;
		this->is_sleeping = false;
		this->has_crashed = false;
		this->has_exited = false;
	}
	void Process::SigSleep()
	{
		this->is_alive = true;
		this->is_running = false;
		this->is_waiting = false;
		this->is_sleeping = true;
		this->has_crashed = false;
		this->has_exited = false;
	}
	void Process::SigWake()
	{
		this->is_alive = true;
		this->is_running = false;
		this->is_waiting = false;
		this->is_sleeping = false;
		this->has_crashed = false;
		this->has_exited = false;
	}
	void Process::SigRun()
	{
		this->is_alive = true;
		this->is_running = true;
		this->is_waiting = false;
		this->is_sleeping = false;
		this->has_crashed = false;
		this->has_exited = false;
	}
	void Process::SigStop()
	{
		this->is_alive = true;
		this->is_running = false;
		this->is_waiting = false;
		this->is_sleeping = false;
		this->has_crashed = false;
		this->has_exited = false;
	}
	void Process::SigKill()
	{
		ProcessManager::Destroy(this);
		this->is_alive = false;
		this->is_running = false;
		this->is_waiting = false;
		this->is_sleeping = false;
		this->has_crashed = false;
		this->has_exited = false;
	}
		
	page_dir_t 		 *Process::GetDir()
	{
		return this->dir;
	}
	const regs32_t   &Process::GetRegs()
	{
		return this->regs;
	}
	const uint8_t    *Process::GetStack()
	{
		return this->stack;
	}
	uint32_t	      Process::GetPID()
	{
		return this->pid;
	}
	PrivilegeLevel    Process::GetPrivilege()
	{
		return this->privilege;
	}
	bool		      Process::IsAlive()
	{
		return this->is_alive;
	}
	bool		      Process::IsRunning()
	{
		return this->is_running;
	}
	bool		      Process::IsWaiting()
	{
		return this->is_waiting;
	}
	bool		      Process::IsSleeping()
	{
		return this->is_sleeping;
	}
	bool		      Process::HasCrashed()
	{
		return this->has_crashed;
	}
	bool		      Process::HasExited()
	{
		return this->has_exited;
	}
	uint32_t 	   	  Process::GetVirtAddr()
	{
		return this->virt_addr;
	}
	uint32_t 	   	  Process::GetPhysAddr()
	{
		return this->phys_addr;
	}
	void 			  Process::SetVirtAddr(uint32_t addr)
	{
		this->virt_addr = addr;
	}


	vector<Process*> ProcessManager::processes_heap = vector<Process*>();
	uint32_t ProcessManager::processes_count = 0;
	uint32_t ProcessManager::processes_free = 0;
	uint32_t ProcessManager::processes_used = 0;
	uint32_t ProcessManager::current_thread = 0;
	uint32_t ProcessManager::next_pid = 0;
	bool 	 ProcessManager::log = true;

	Process *ProcessManager::GetFreeProcEntry()
	{
		Process *result = (Process*)System::Userland::UserHeap.AllocEntries(PROC_ENTRIES_COUNT);
		processes_heap.push_back(result);
		return (Process*)processes_heap[processes_count];
	}

	Process *ProcessManager::Create(uint32_t vaddr, uint32_t paddr, ProcessEntry entry, PrivilegeLevel privilege, page_dir_t *dir)
	{
		Process *proc = GetFreeProcEntry();

		proc->Init(vaddr, paddr, entry, privilege);
		proc->pid = processes_count;

		if (dir) { proc->dir = dir; goto done; }

		page_init_dir(proc->dir);
		
		page_map_addr_dir_sz((uint32_t)proc, (uint32_t)proc, proc->dir, sizeof(Process), (page_table_entry_t){
			.present = true,
			.rw = read_write,
			.privilege = user,
			.reserved_1 = 0,
			.accessed = false,
			.dirty = false,
			.reserved_2 = 0,
			.free = false,
			.unused = 0,
		});

		done:
			if (log) dprintf("[ProcessManager] Process with PID %d created at address 0x%x\n", proc->GetPID(), proc);
			if (log) dprintf("	- Page Dir: 0x%x\n", proc->GetDir());
			processes_count++;
			processes_used++;
			return proc;
	}
	void ProcessManager::Destroy(Process *process)
	{
		if (process->IsAlive())
		{
			System::Userland::UserHeap.FreeAllProcessEntries(process->GetPID());
			processes_heap.remove(process->GetPID());
			process->has_crashed = false;
			process->has_exited = false;
			process->is_alive = false;
			process->is_running = false;
			process->is_waiting = false;
			process->is_sleeping = false;
			process->dir = NULL;
			
			memset(process->stack, 0, sizeof(process->stack));
			memset(process->pad, 0, sizeof(process->pad));

			System::Userland::UserHeap.FreeEntries(process, PROC_ENTRIES_COUNT);

			ProcessManager::processes_used--;
			ProcessManager::processes_count--;

			if (log) dprintf("[ProcessManager] Process with PID %d destroyed!\n", process->GetPID());
		}
	}
	void ProcessManager::Destroy(uint32_t pid)
	{
		Process *proc = processes_heap[pid];
		if (proc)
		{
			Destroy(proc);
		}
	}
	
	bool started = false;

	void ProcessManager::SwitchProcess(regs32_t *r)
	{
		asm("cli");
		page_switch_dir(get_kernel_dir());
		if (ProcessManager::processes_count == 0) return;
		Process *proc = ProcessManager::processes_heap[ProcessManager::current_thread];
		if(started) proc->regs = *r;
		int i = 0;
		loop:
			if (i > ProcessManager::processes_count)
			{
				ProcessManager::current_thread = 0;
				return;
			}
			if (++ProcessManager::current_thread >= ProcessManager::processes_count)
			{
				ProcessManager::current_thread = 0;
			}

			proc = ProcessManager::processes_heap[ProcessManager::current_thread];
			if (log) dprintf("[ProcessManager] PID: %d\n", proc->GetPID());
			if (proc->IsAlive())
			{
				if (!proc->IsRunning()) proc->SigRun();
				page_switch_dir(proc->GetDir());
				
				r->eax = proc->regs.eax;
				r->ebx = proc->regs.ebx;
				r->ecx = proc->regs.ecx;
				r->edx = proc->regs.edx;
				
				r->esi = proc->regs.esi;
				r->edi = proc->regs.edi;
				r->ebp = proc->regs.ebp;
				
				if(proc->privilege == PrivilegeLevel::User) r->useresp = proc->regs.esp;
				r->eip = proc->regs.eip;

				r->cs = proc->regs.cs;
				r->ds = proc->regs.ds;

				r->eflags = proc->regs.eflags;
				r->ss = proc->regs.ss;
				
				r->identifier.interrupt_number = proc->regs.identifier.interrupt_number;

				if (log)
				{
					dprintf("[ProcessManager] Switching to process %d/%d\n", proc->GetPID()+1, ProcessManager::processes_count);
					dprintf("[ProcessManager] CS: 0x%x, DS: 0x%x, SS: 0x%x\n", r->cs, r->ds, r->ss);
				}
			}
			else if (proc->HasExited() || proc->HasCrashed())
			{
				ProcessManager::Destroy(proc);
				if (log) dprintf("[ProcessManager] Process %d has exited\n", proc->GetPID(), ProcessManager::processes_count);
				goto loop;
			}
			else { i++; goto loop; }
		started = true;
	}
	void ProcessManager::Init()
	{
	//	ProcessManager::processes_count = 0;
	//	ProcessManager::processes_free = 0;
	//	ProcessManager::processes_used = 0;
	//	ProcessManager::current_thread = 0;
	//	ProcessManager::next_pid = 0;
		register_int_handler(0, ProcessManager::SwitchProcess);
	}
	void ProcessManager::ToggleLog()
	{
		ProcessManager::log = !ProcessManager::log;
	}
	Process *ProcessManager::GetCurrentProcess()
	{
		return ProcessManager::processes_heap[ProcessManager::current_thread];
	}
	bool ProcessManager::IsStarted()
	{
		return started;
	}
}