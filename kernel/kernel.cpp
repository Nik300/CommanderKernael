//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

//TODO: aggiungere supporto a più lingue

#include <Headers/environment.hpp>
#include <Headers/core.hpp>
#include <Headers/drivers/keyboard.hpp>
#include <Headers/drivers/multitask_timer.hpp>
#include <Headers/keyboards/languages.hpp>
#include <Headers/util.hpp>
#include <Headers/stdio.hpp>
#include <Headers/shell.hpp>
#include <Headers/ports.hpp>
#include <Headers/paging.hpp>
#include <Headers/constructors.hpp>
#include <Headers/multitasking.hpp>
#include <Headers/drivers/cs_socket.hpp>
#include <Headers/drivers/vesa.hpp>

#include <libc/random.hpp>

using namespace CommanderKernael;
using namespace core;
using namespace Drivers;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace CommanderKernael::multitasking;

using namespace System::Random;

extern "C" {
	extern multiboot_header_t* mbootdat;
}

void TaskA(){
	shell* s = new shell(new Console());
	s->start();
}
void KernelShellCrashHandle(int pid, Task* task, string error, string type, string from, Console* console);
void KernelCrashHandle(int pid, Task* task, string error, string type, string from){ 
	System::console->takeOwnership();
	System::console->clear();
	System::console->printf("Shell was reloaded due to error \"");
	System::console->printf(error);
	System::console->println("\".");
	taskManager->AddTask(new Task(4, TaskA, new string("shell"), KernelCrashHandle, KernelShellCrashHandle, false));
}
void KernelShellCrashHandle(int pid, Task* task, string error, string type, string from, Console* console){
	System::console->takeOwnership();
	System::console->clear();
	System::console->printf("Shell was reloaded due to error \"");
	System::console->printf(error);
	System::console->println("\".");
	taskManager->AddTask(new Task(4, TaskA, new string("shell"), KernelCrashHandle, KernelShellCrashHandle, false));
}
void KernelCrashHandle(int pid, Task* task, string error, string type, string from, Console* console){
	error::panic(error, type, from);
}

namespace CommanderKernael{
	namespace main{
		class kernel{
		private:
			int bootStatus;
			stdio::Console console;
			bool Hang = false;
		public:
			kernel(int status){
				this->bootStatus = status;
				this->console = stdio::Console();
			}
			~kernel(){
				this->bootStatus = 0;
			}
			void abort(){
				this->console.clear();
				core::cpu::interrupts::call<3>();
			}
			void log(string text) {
				console.takeOwnership();
				ConsoleColor oldF = console.getForegroundColor();
				console.setForegroundColor(ConsoleColor::Green);
				console.printf("[Kernel] ");
				console.setForegroundColor(ConsoleColor::DarkGray);
				console.println(text._iterator);
				console.setForegroundColor(oldF);
				core::debug::send_string(SERIAL_PORT_A, "[Kernel] " + text);
				core::debug::send_string(SERIAL_PORT_A, "\n");
			}
			void warn(string text) {
				console.takeOwnership();
				ConsoleColor oldF = console.getForegroundColor();
				console.setForegroundColor(ConsoleColor::Yellow);
				console.printf("[Kernel: Warning] ");
				console.setForegroundColor(ConsoleColor::LightGray);
				console.println(text._iterator);
				console.setForegroundColor(oldF);
				core::debug::send_string(SERIAL_PORT_A, "[Kernel: Warning] " + text);
				core::debug::send_string(SERIAL_PORT_A, "\n");
			}
			void bootInfo(string text) {
				console.takeOwnership();
				ConsoleColor oldF = console.getForegroundColor();
				console.setForegroundColor(ConsoleColor::Brown);
				console.printf("[Boot Info] ");
				console.setForegroundColor(ConsoleColor::Cyan);
				console.println(text._iterator);
				console.setForegroundColor(oldF);
			}
			static void Kernel(){
				taskManager->AddTask(new Task(4, TaskA, new string("shell"), (multitasking::Task::errorHandler)KernelShellCrashHandle, KernelShellCrashHandle, false));
				taskManager->AddTask(new Task(1, [](){ while(true) { GlobalSeed++; } }, new string("random_seed_updater")));
				while (true);
			}
			void start(){
				if(this->bootStatus != 1){
					abort();
					return;
				}
				this->console.clear();
				delete CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
				CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme = new CommanderKernael::keyboardSchemes::Italian(&this->console);
				delete CommanderKernael::keyboardSchemes::scelectedScheme;
				CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
				this->console.clear();
				this->bootStatus = 2;
				this->console.takeOwnership();
				this->console.clear();
				this->log("booted successfully");
				this->log("installing serial debug");
				core::debug::install();
				this->log("serial debug installed");
				this->log("Initializing Advanced Power Manager...");
				if (power::APM::Check()) { power::APM::Connect(); this->log("Advanced Power Manager initialized and connected!"); }
				//else this->warning("Advanced Power Manager is either not supported or not enabled. You'll be unable to shutdown the machine!");
				this->bootInfo(string("Bootloader: ") + (String)mbootdat->boot_loader_name);
				this->bootInfo(string("lower mem: ") + intToASCII((uint32_t)mbootdat->mem_lower));
				this->bootInfo(string("upper mem: ") + intToASCII((uint32_t)mbootdat->mem_upper));
				this->bootInfo(string("Available video mode: ") + (String)mbootdat->vbe_control_info);
				/*this->log("Enabling paging...");
				StartPaging();
				this->log("Paging enabled successfully!");*/
				taskManager = new TaskManager();
				taskManager->AddTask(new Task(6, Kernel, new string("kernel"), (multitasking::Task::errorHandler)KernelCrashHandle, KernelCrashHandle, false));
				//taskManager->AddTask(new Task(1, [](){ while(true) if(!cpu::interrupts::State()) __asm__ volatile("sti"); }, new string("Interrupts_manager")));
				//this->log("Shell has been loaded into thread 1");
				//taskManager->AddTask(new Task(4, TaskB));
				//taskManager->AddTask(new Task(4, TaskC));
				//this->log("loading interrupts...");
				cpu::interrupts::install();
				cpu::interrupts::enable();
				//this->log("interrupts enabled!");
				timer = new MultitaskingTimer(taskManager, 1);
				this->log("Injecting multitasking...");
				timer->activate();
				this->hang(); 
			}
			void hang(){
                this->Hang = true;
				while(Hang){
                    if (this->console.hasOwnership() != true)
                        this->console.takeOwnership();
				}
			}
			void resume(){
                this->Hang = false;
                this->console.takeOwnership();
			}
		};
	}
}

extern "C" void assemblyEntry()
{
	CommanderKernael::main::kernel CommanderKernael = CommanderKernael::main::kernel(1);
	CommanderKernael.start();
}
