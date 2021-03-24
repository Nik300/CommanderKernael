#ifndef __COMMANDERKERNAEL_HANDLING_ERRORS_HPP
#define __COMMANDERKERNAEL_HANDLING_ERRORS_HPP

#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <libc/console.hpp>

using namespace CommanderKernael::Drivers::stdio;
using namespace assembly::types;

using namespace System;

namespace CommanderKernael{
	namespace core{
		namespace debug {
			void install();
			int receive_32(int device);
			char receive_8(int device);
			char receive_8_async(int device);
			int transmit_empty(int device);
			void send(int device, char out);
			void send_string(int device, String out);
			void send_string(int device, string out);
		}
	}
}
namespace CommanderKernael {
	namespace main {
		void WaitForRender();
		class error {
		public:
			static void panic(string error, string type, string from) {
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, "[Error handling] A kernel panic was triggered.\n");
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, "Type: ");
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, type);
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, "\n");
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, "From: ");
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, from);
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, "\n");
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, "Error: ");
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, error);
				CommanderKernael::core::debug::send_string(SERIAL_PORT_A, "\n");
				System::console->setBackgroundColor(ConsoleColor::Blue);
				System::console->setForegroundColor(ConsoleColor::White);
				System::console->clear();
				System::console->printf("A ");
				System::console->printf(type._iterator);
				System::console->println(" occurred and CommanderKernael has been shut down to prevent damages to your PC.");
				System::console->printf("\nError: ");
				System::console->setForegroundColor(ConsoleColor::Red);
				System::console->println(error._iterator);
				System::console->setForegroundColor(ConsoleColor::White);
				System::console->printf("From: ");
				System::console->println(from._iterator);
				System::console->println("\nA detailed stack report will be stored into the hard disk when ATA will be supported.");
				System::console->println("By now you are fucked up because there's no documentation about errors. LOL!");
				System::console->println("\nHalting CPU...");
				System::console->disableCursor();
				System::console->println("System halted!");
				WaitForRender();
				asm("cli");
				asm("hlt");
				while (true);
			}
		};
	}
}

#endif //__COMMANDERKERNAEL_HANDLING_ERRORS_HPP