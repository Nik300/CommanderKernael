#pragma once

#ifndef __cplusplus
#error "This header is C++ only."
#endif

#include <stdint.h>
#include <stddef.h>

#include <std/stddecl.h>
#include <std/int_handler.h>

#include <lib/paging.h>
#include <std/vector.hpp>

namespace System::Tasking
{
	typedef void (*ProcessEntry)(char *argv[], int argc);
	enum class PrivilegeLevel : uint8_t
	{
		Kernel = 0,
		Module = 1,
		Driver = 2,
		User   = 3
	};
	class Process final
	{
	friend class ProcessManager;
	private:
		regs32_t   regs;
		uint8_t	   stack[64*1024];
	private:
		page_dir_t   *dir;
		uint8_t	  	 pad[4096+sizeof(page_dir_t)];
		uint32_t   	 virt_addr;
		uint32_t   	 phys_addr;
		ProcessEntry entry;
	private:
		bool 	   is_alive   : 1;
		bool 	   is_running : 1;
		bool 	   is_waiting : 1;
		bool 	   is_sleeping: 1;
	private:
		bool 	   has_crashed: 1;
		bool 	   has_exited : 1;
	private:
		uint32_t   pid;
		PrivilegeLevel privilege;

	public:
		Process(uint32_t vaddr, uint32_t paddr, ProcessEntry entry, PrivilegeLevel privilege);
		~Process();
	public:
		void SigExit();
		void SigCrash();
		void SigWait();
		void SigSleep();
		void SigWake();
		void SigRun();
		void SigStop();
		void SigKill();
	public:
		page_dir_t 		 *GetDir();
		const regs32_t   &GetRegs();
		const uint8_t    *GetStack();
		uint32_t	      GetPID();
		PrivilegeLevel    GetPrivilege();
		bool		      IsAlive();
		bool		      IsRunning();
		bool		      IsWaiting();	
		bool		      IsSleeping();
		bool		      HasCrashed();
		bool		      HasExited();
		uint32_t		  GetVirtAddr();
		uint32_t		  GetPhysAddr();
		void			  SetVirtAddr(uint32_t vaddr);
	public:
		void              Init(uint32_t vaddr, uint32_t paddr, ProcessEntry entry, PrivilegeLevel privilege);
	};
	class ProcessManager final
	{
	private:
		static vector<Process*> processes_heap;
		static uint32_t processes_count;
		static uint32_t processes_free;
		static uint32_t processes_used;
		static uint32_t current_thread;
		static uint32_t next_pid;
		static bool 	log;
	private:
		static Process 	*GetFreeProcEntry();

	public:
		static Process *Create(uint32_t vaddr, uint32_t paddr, ProcessEntry entry, PrivilegeLevel privilege, page_dir_t *dir = nullptr);
		static void Destroy(Process *process);
		static void Destroy(uint32_t pid);
	public:
		static void SwitchProcess(regs32_t *regs);
		static void Init();
		static void ToggleLog();
		static Process *GetCurrentProcess();
		static bool IsStarted();
	};
}