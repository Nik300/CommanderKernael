#include <lib/idt.hpp>
#include <lib/gdt.h>
#include <lib/ports.h>
#include <lib/memory.h>
#include <std/stdio.h>
#include <lib/serial.h>
#include <init/modules.h>
#include <lib/process.hpp>

#define LOW_16(x) (uint16_t)(x & 0xFFFF)
#define HIGH_16(x) (uint16_t)(x >> 16)

extern "C" void isr_0();
extern "C" void isr_1();
extern "C" void isr_2();
extern "C" void isr_3();
extern "C" void isr_4();
extern "C" void isr_5();
extern "C" void isr_6();
extern "C" void isr_7();
extern "C" void isr_8();
extern "C" void isr_9();
extern "C" void isr_10();
extern "C" void isr_11();
extern "C" void isr_12();
extern "C" void isr_13();
extern "C" void isr_14();
extern "C" void isr_15();
extern "C" void isr_16();
extern "C" void isr_17();
extern "C" void isr_18();
extern "C" void isr_19();
extern "C" void isr_20();
extern "C" void isr_21();
extern "C" void isr_22();
extern "C" void isr_23();
extern "C" void isr_24();
extern "C" void isr_25();
extern "C" void isr_26();
extern "C" void isr_27();
extern "C" void isr_28();
extern "C" void isr_29();
extern "C" void isr_30();
extern "C" void isr_31();

extern "C" void irq_0();
extern "C" void irq_1();
extern "C" void irq_2();
extern "C" void irq_3();
extern "C" void irq_4();
extern "C" void irq_5();
extern "C" void irq_6();
extern "C" void irq_7();
extern "C" void irq_8();
extern "C" void irq_9();
extern "C" void irq_10();
extern "C" void irq_11();
extern "C" void irq_12();
extern "C" void irq_13();
extern "C" void irq_14();
extern "C" void irq_15();
extern "C" void irq_16();

extern "C" idt_register_t *_idtr = nullptr;
extern "C" idt32_gate 	  idt32_table[256] = {0};
extern "C" handler_t      *handler_table = nullptr;

extern "C" void __idt_flush();

extern "C" void *idt_table;

const idt32_gate& idt32_set_gate(uint8_t n, uintptr_t handler, uint8_t access_ring, idt32_gate_t gate_type, uint16_t segment)
{
	idt32_table[n].offset_low = LOW_16(handler);
	idt32_table[n].offset_high = HIGH_16(handler);

	idt32_table[n].UNUSED = 0;

	idt32_table[n].DPL = access_ring;
	idt32_table[n].type = gate_type;
	idt32_table[n].present = true;

	idt32_table[n].segment = segment;
	return idt32_table[n];
}
bool idt32_init()
{
	_idtr = new idt_register_t();

	handler_table = new handler_t[256-32];

	// install isrs
	idt32_set_gate(0,  (uintptr_t)isr_0,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(1,  (uintptr_t)isr_1,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(2,  (uintptr_t)isr_2,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(3,  (uintptr_t)isr_3,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(4,  (uintptr_t)isr_4,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(5,  (uintptr_t)isr_5,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(6,  (uintptr_t)isr_6,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(7,  (uintptr_t)isr_7,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(8,  (uintptr_t)isr_8,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(9,  (uintptr_t)isr_9,  3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(10, (uintptr_t)isr_10, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(11, (uintptr_t)isr_11, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(12, (uintptr_t)isr_12, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(13, (uintptr_t)isr_13, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(14, (uintptr_t)isr_14, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(15, (uintptr_t)isr_15, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(16, (uintptr_t)isr_16, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(17, (uintptr_t)isr_17, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(18, (uintptr_t)isr_18, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(19, (uintptr_t)isr_19, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(20, (uintptr_t)isr_20, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(21, (uintptr_t)isr_21, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(22, (uintptr_t)isr_22, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(23, (uintptr_t)isr_23, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(24, (uintptr_t)isr_24, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(25, (uintptr_t)isr_25, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(26, (uintptr_t)isr_26, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(27, (uintptr_t)isr_27, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(28, (uintptr_t)isr_28, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(29, (uintptr_t)isr_29, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(30, (uintptr_t)isr_30, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	idt32_set_gate(31, (uintptr_t)isr_31, 3, idt32_gate_t::TRAP_32, get_kernel_code_segment());
	
	// remap PIC
	outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

	// install irqs
	idt32_set_gate(32, (uintptr_t)irq_0, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(33, (uintptr_t)irq_1, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(34, (uintptr_t)irq_2, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(35, (uintptr_t)irq_3, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(36, (uintptr_t)irq_4, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(37, (uintptr_t)irq_5, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(38, (uintptr_t)irq_6, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(39, (uintptr_t)irq_7, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(40, (uintptr_t)irq_8, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(41, (uintptr_t)irq_9, 3,	idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(42, (uintptr_t)irq_10,3,idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(43, (uintptr_t)irq_11,3,idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(44, (uintptr_t)irq_12,3,idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(45, (uintptr_t)irq_13,3,idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(46, (uintptr_t)irq_14,3,idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());
	idt32_set_gate(47, (uintptr_t)irq_15,3,idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());

	// install syscall
	idt32_set_gate(48, (uintptr_t)irq_16, 3, idt32_gate_t::INTERRUPT_32, get_kernel_code_segment());

	_idtr->base = (uintptr_t) idt32_table;
    _idtr->limit = (256 * sizeof(idt32_gate));
	__reload_regs();
	__idt_flush();

	// install syscall handler
	register_int_handler(16, [](regs32_t *r){
		switch (r->eax)
		{
			case 0:
				printf((char*)r->ebx);
				break;
			case 1:
				using namespace System::Tasking;
				// exit system call
				page_switch_dir(get_kernel_dir());
				__reload_regs();
				ProcessManager::Destroy(ProcessManager::GetCurrentProcess());
			default:
				r->eax = -1;
		}
	});

	return true;
}

const char* exceptions[] = {
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

extern "C" void fault_handler(regs32_t regs)
{
	asm volatile ("cli");
	__tss.esp0 = (uintptr_t)regs.esp;

	using System::IO::Console;
	using System::IO::ConsoleColor;
	using namespace System::IO;

	//set bsod
	//Console::Clear(ConsoleColor::White, ConsoleColor::Blue);

	//print exception
	Serial::WriteLine("An error has occured and the kernel will be halted to prevent further damage.\n\nTechnical details:");
	Serial::WriteLine("Exception: %s", exceptions[regs.identifier.trap_number]);
	Serial::WriteLine("Error code: %x", regs.eflags);

	//stack trace
	Serial::WriteLine("\nStack trace:");
	Serial::WriteLine("EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x", regs.eax, regs.ebx, regs.ecx, regs.edx);
	Serial::WriteLine("ESI: 0x%x EDI: 0x%x EBP: 0x%x ESP: 0x%x", regs.esi, regs.edi, regs.ebp, regs.esp);
	Serial::WriteLine("EFLAGS: %x", regs.eflags);
	Serial::WriteLine("CS: 0x%x DS: 0x%x SS: 0x%x", regs.cs, regs.ds, regs.ss);
	Serial::WriteLine("EIP: 0x%x", regs.eip);
	Serial::WriteLine("USER ESP: 0x%x", regs.useresp);

	// send eoi
	outb(0x20, 0x20);

	asm volatile ("hlt");
	for(;;);
}
extern "C" void interrupt_handler(regs32_t regs)
{
	__tss.esp0 = (uintptr_t)regs.esp;

	if (handler_table[regs.identifier.interrupt_number])
	{
		handler_table[regs.identifier.interrupt_number](&regs);
	}

	// send eoi
	if (regs.identifier.interrupt_number >= 8)
	{
		outb(0xA0, 0x20);
	}
	outb(0x20, 0x20);

	gdt32_table[TSS_INDEX].read_write = 0;
}

extern "C" void register_int_handler(uint8_t irq, handler_t handler)
{
	handler_table[irq] = handler;
}

void enable_interrupts()
{
	using namespace System::IO;
	asm volatile ("sti");
	Console::WriteLine("enable_interrupts() >> interrupts enabled");
}