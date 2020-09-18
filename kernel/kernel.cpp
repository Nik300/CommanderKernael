//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//this files.

//See CONTRIBUTORS.TXT for more infos

//TODO: aggiungere supporto a più lingue

#include <Headers/environment.hpp>
#include <Headers/core.hpp>
#include <Headers/drivers/keyboard.hpp>
#include <Headers/keyboards/languages.hpp>
#include <Headers/util.hpp>
#include <Headers/stdio.hpp>
#include <Headers/shell.hpp>

using namespace CommanderKernael;
using namespace core;
using namespace Drivers;
using namespace CommanderKernael::utilities::ASCII_UTIL;

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
				this->console.println("[kernel] MUSO MARSO! Vai a controllare il PC!");
			}
			void start(){
				if(this->bootStatus != 1){
					abort();
					return;
				}
				this->console.clear();
				CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme = CommanderKernael::keyboardSchemes::Italian(&this->console);
				CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
				CommanderKernael::Drivers::keyboardDriver kdriver = CommanderKernael::Drivers::keyboardDriver();
				this->console.clear();
				this->bootStatus = 2;
				this->console.takeOwnership();
				this->console.println("[kernel] t'apposto tranqui, so partitio!");
				cpu::interrupts::install();
				cpu::interrupts::enable();
				this->console.println("[kernel] interrupts enabled!");
				kdriver.activate();
				this->console.println("[kernel] Keyboard driver enabled!");
				shell _shell = shell(&this->console);
				_shell.start();
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
