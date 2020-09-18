#ifndef __COMMANDERKERNAEL_CORE_CORE_HPP
#define __COMMANDERKERNAEL_CORE_CORE_HPP

//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//this files.

//See CONTRIBUTORS.TXT for more infos

#include "Headers/types.hpp"
#include "Headers/stdio.hpp"
#include "Headers/util.hpp"
using namespace assembly::types;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;

//API ESPORTATE DAL C
extern "C"{
	typedef struct{
		uint32_t ds;
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		int int_no, err_code;
		uint32_t eip, cs, eflags, useresp, ss;
	}registers_t;
	typedef void (*isr_t)(registers_t);
	void isr_install();
	void register_interrupt_handler(uint8_t, isr_t);
}
namespace CommanderKernael{
	namespace core{
		namespace cpu{
			String exception_messages[] = {
			    "Division By Zero",
			    "Debug",
			    "Non Maskable Interrupt",
			    "Breakpoint",
			    "Into Detected Overflow",
			    "Out of Bounds",
			    "Invalid Opcode",
			    "No Coprocessor",

			    "Double Fault",
			    "Coprocessor Segment Overrun",
			    "Bad TSS",
			    "Segment Not Present",
			    "Stack Fault",
			    "General Protection Fault",
			    "Page Fault",
			    "Unknown Interrupt",

			    "Coprocessor Fault",
			    "Alignment Check",
			    "Machine Check",
			    "Reserved",
			    "Reserved",
			    "Reserved",
			    "Reserved",
			    "Reserved",

			    "Reserved",
			    "Reserved",
			    "Reserved",
			    "Reserved",
			    "Reserved",
			    "Reserved",
			    "Reserved",
			    "Reserved"
			};
			class iRQ{
			public:
				static bool addIRQHandler(uint8_t iRQNumber, isr_t handler, bool output = true){
					register_interrupt_handler((uint8_t)(iRQNumber + 32), handler);
					return output;
				}
			};
			class interrupts{
			public:
				static bool install(bool output = true){
					isr_install();
					return output;
				}
				static bool enable(bool output = true){
					asm volatile("sti");
					return output;
				}
				static bool disable(bool output = true){
					asm volatile("cli");
					return output;
				}
				template <uint8_t N> static inline void call(void)
				{
					__asm__ ("int %0\n" : : "N"(N) : );
				}
				static bool addInterrupt(uint8_t number, isr_t handler, bool output = true){
					register_interrupt_handler(number, handler);
					return output;
				}
			};
		}
	}
}
//isr_handler
extern "C" void isr_handler(registers_t r){
	CommanderKernael::core::cpu::interrupts::disable(false);
	Console console = Console();
	console.clear();
	console.println("###SYSTEM ERROR###");
	console.printf("Name: ");
	console.println(CommanderKernael::core::cpu::exception_messages[(int)(r.int_no)]);
	console.println("type: \"hardware failure\"");
	console.println("from: \"CPU\"");
	console.println("\nSTACK REPORT:");
	console.printf("DS: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.ds));
    console.printf("EDI: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.edi));
	console.printf("ESI: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.esi));
	console.printf("EBP: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.ebp));
	console.printf("ESP: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.esp));
	console.printf("EBX: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.ebx));
	console.printf("EDX: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.edx));
	console.printf("ECX: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.ecx));
	console.printf("EAX: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.eax));
	console.printf("EIP: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.eip));
	console.printf("CS: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.cs));
	console.printf("EFLAGS: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.eflags));
	console.printf("USERESP: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.useresp));
	console.printf("SS: ");
	console.println(CommanderKernael::utilities::ASCII_UTIL::intToASCII(r.ss));
	console.println("###SYSTEM ERROR###");
	while(true);
}

#endif //__COMMANDERKERNAEL_CORE_CORE_HPP
