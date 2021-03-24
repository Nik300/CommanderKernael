#ifndef __COMMANDERKERNAEL_CORE_CORE_HPP
#define __COMMANDERKERNAEL_CORE_CORE_HPP

#define SERIAL_PORT_A 0x3F8
#define SERIAL_PORT_B 0x2F8
#define SERIAL_PORT_C 0x3E8
#define SERIAL_PORT_D 0x2E8

//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#include <Headers/types.hpp>
#include <Headers/stdio.hpp>
#include <Headers/util.hpp>
#include <Headers/errors.hpp>

using namespace assembly::types;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::main;

//API ESPORTATE DAL C
extern "C"{
	typedef struct{
		uint32_t ds;
		uint32_t edi, esi, ebp, ebx, edx, ecx, eax;
		int int_no, err_code;
		uint32_t eip, cs, eflags, useresp, ss;
	}registers_t;
	typedef void (*isr_t)(uint32_t*);
	void isr_install();
	void register_interrupt_handler(uint8_t, isr_t);
	uint32_t count_memory();
	extern int kernel_end;  //	linker.ld
	extern int kernel_stack;//	source/entry/kernel_entry.asm
	extern uint32_t placement_address;
	extern void kern_mem(); //	source/entry/kernel_entry.asm
	extern void long_mem(); //	source/entry/kernel_entry.asm

	//serial debugging
	extern void serial_install();
	extern int serial_rcvd(int device);
	extern char serial_recv(int device);
	extern char serial_recv_async(int device);
	extern int serial_transmit_empty(int device);
	extern void serial_send(int device, char out);
	extern void serial_string(int device, char* out);

	struct regs16_t {
		unsigned short di, si, bp, sp, bx, dx, cx, ax;
		unsigned short gs, fs, es, ds, eflags;
	} __attribute__((packed));

	extern void int32(unsigned char intnum, regs16_t* regs);
	uint32_t e_kmalloc_a(uint32_t sz);
	uint32_t e_kmalloc_p(uint32_t sz, uint32_t* phys);
	uint32_t e_kmalloc_ap(uint32_t sz, uint32_t* phys);
	uint32_t e_kmalloc(uint32_t sz);
	extern uint32_t placement_address;
}
namespace CommanderKernael{
	namespace core{
		namespace debug {
			void install() { serial_install(); }
			int receive_32(int device) { return serial_rcvd(device); }
			char receive_8(int device) { return serial_recv(device); }
			char receive_8_async(int device) { return serial_recv_async(device); }
			int transmit_empty(int device) { return serial_transmit_empty(device); }
			void send(int device, char out) { serial_send(device, out); }
			void send_string(int device, String out) { serial_string(device, out); }
			void send_string(int device, string out) { serial_string(device, out._iterator); }
		}
		namespace memory{
			void* allocateMemory(int size);
			void* allocateMemory(int size, void* data);
			void releaseMemory(void* content, int size);
			void* NULL = (void*)0x0000;
			template<typename mType> class MemoryBlock {
			public:
				uint8_t* address;
				int size;
				MemoryBlock(int size) {
					address = (uint8_t*)allocateMemory(size * sizeof(mType));
					this->size = size;
				}
				MemoryBlock(int size, mType zbt) {
					for (int i = 0; i < size; i++) {
						address = (uint8_t*)allocateMemory(sizeof(mType), (void*)&zbt);
					}
					this->size = size;
				}
				MemoryBlock(uint32_t address, int size) {
					this->address = (uint8_t*)address;
					this->size = size;
				}
				MemoryBlock() {
					this->address = (uint8_t*)NULL;
					this->size = 0;
				}
				mType Read(int pos) {
					if (pos >= size || pos < 0) {
						int foo = 1/0;
						return *(mType*)NULL;
					}
					return ((mType*)address)[pos];
				}
				void Write(int pos, mType content) {
					if (pos >= size || pos < 0) return;
					((mType*)address)[pos] = content;
				}
				void WriteAll(mType* content, int size) {
					if (size > this->size || size < 0) return;
					CommanderKernael::utilities::memory::memcpy((CommanderKernael::utilities::memory::content_t)content, (CommanderKernael::utilities::memory::content_t)address, size);
				}
				mType* ToList() {
					return (mType*)address;
				}
				template<typename T> T Read(int pos) {
					if (sizeof(T) > sizeof(mType) || pos >= size * (sizeof(mType) / sizeof(T)) || pos < 0) return *(T*)NULL;
					return ((T*)address)[pos];
				}
				template<typename T> void Write(int pos, T content) {
					if (sizeof(T) > sizeof(mType) || pos >= size * (sizeof(mType) / sizeof(T)) || pos < 0) return;
					((T*)address)[pos] = content;
				}
				template<typename T> void WriteAll(T* content, int size) {
					if (sizeof(T) > sizeof(mType) || size > this->size * (sizeof(mType) / sizeof(T)) || size < 0) return;
					CommanderKernael::utilities::memory::memcpy((CommanderKernael::utilities::memory::content_t)content, (CommanderKernael::utilities::memory::content_t)address, size);
				}
				template<typename T> T* ToList() {
					return (T*)address;
				}
				mType* operator[](int pos) {
					if (pos >= size || pos < 0) return (mType*)NULL;
					return &((mType*)address)[pos];
				}
			};
			uint8_t* memzone = (uint8_t*)&kern_mem;
			uint8_t* longmemzone = (uint8_t*)&long_mem;
			uint8_t* unmanagedZone = (uint8_t*)0x1000;
			uint32_t maxMemory;
			uint32_t GetMemory(){
				return count_memory();
			}
			void initMemory(int size) {
				maxMemory = count_memory();
				if ((uint32_t)memzone + size > GetMemory() * 1024 * 1024) {
					error::panic("Memory request exceeded resources!", "Fatal Error\0", "Memory Manager\0");
				}
				CommanderKernael::utilities::memory::memset((uint8_t)0x00, (CommanderKernael::utilities::memory::content_t)kernel_end, size);
			}
			void* allocateLongMemory(int size) {
				void* alloc = longmemzone;
				longmemzone += size+1;
				return alloc;
			}
			void* allocateMemory(int size) {
				uint8_t* alloc;
				alloc = (uint8_t*)e_kmalloc(size+1);
				alloc[size] = 0;
				return (void*)alloc;
			}
			void* allocateMemory_old(int size) {
				uint8_t* alloc;
				alloc = (uint8_t*)memzone;
				memzone += size;
				*(uint8_t*)e_kmalloc(1) = 0;
				return (void*)alloc;
			}
			void* allocateMemory(int size, bool force) {
				return allocateMemory(size);
			}
			void* allocateMemory(int size, void* data) {
				uint8_t* alloc;
				alloc = (uint8_t*)e_kmalloc(size);
				*(uint8_t*)e_kmalloc(1) = 0;
				CommanderKernael::utilities::memory::memcpy((CommanderKernael::utilities::memory::content_t)data, (CommanderKernael::utilities::memory::content_t)alloc, size);
				return (void*)alloc;
			}
			void* allocateUnmanagedMemory(int size) {
				uint8_t* alloc;
				alloc = (uint8_t*)e_kmalloc(size);
				*(uint8_t*)e_kmalloc(1) = 0;
				return (void*)alloc;
			}
			void clearMemory(void* data) {
				int size = util::sizeofstr((char*)data);
				CommanderKernael::utilities::memory::memset((uint8_t)0x00, (CommanderKernael::utilities::memory::content_t)data, size);
			}
			void clearMemory(void* data, int size) {
				CommanderKernael::utilities::memory::memset((uint8_t)0x00, (CommanderKernael::utilities::memory::content_t)data, size);
			}
			void releaseMemory(void* content) {
				int size = util::sizeofstr((char*)content);
				CommanderKernael::utilities::memory::memset(0, (CommanderKernael::utilities::memory::content_t)memzone, size);
			}
			void releaseMemory(void* content, int size) {
				CommanderKernael::utilities::memory::memset(0, (CommanderKernael::utilities::memory::content_t)memzone, size);
			}
			uint32_t memoryUsage() {
				int free = 0;
				int used = 0;
				for (int i = kernel_end; i < placement_address; i++)
				{
					int data = *(int*)(i);
					if (data == 0)
						free++;
					else
						used++;
				}
				return used/1024;
			}
			uint32_t kernelUsage() {
				return (uint32_t)(&kernel_end);
			}
		}
		namespace cpu{
			uint32_t GDTCodeSegment = 0;
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
				static bool irq_ack(int irq_no, bool output = true) {
					if (irq_no >= 12) {
						port8bit p(0xA0);
						p.write(0x20);
					}
					port8bit p(0x20);
					p.write(0x20);
					return output;
				}
				static bool install(bool output = true){
					isr_install();
					return output;
				}
				static bool enable(bool output = true){
					asm("sti");
					return output;
				}
				static bool disable(bool output = true){
					asm("cli");
					return output;
				}
				static bool State(){
					unsigned long flags;
					asm volatile ( "pushf\n\t"
								"pop %0"
								: "=g"(flags) );
					return flags & (1 << 9);
				}
				template <uint8_t N> static inline void call(void)
				{
					__asm__ ("int %0\n" : : "N"(N) : );
				}
				template <uint8_t N> static inline void call16(regs16_t* regs)
				{
					int32(N, regs);
				}
				static bool addInterrupt(uint8_t number, isr_t handler, bool output = true){
					register_interrupt_handler(number, handler);
					enable(output);
					return output;
				}
			};
			class functions {
			public:
				static void halt() {
					asm("hlt");
				}
				static void reboot(char* args)
				{
					port8bit p1 = port8bit(0x64);
					port8bit p2 = port8bit(0x60);
					unsigned temp;
					do
					{
						temp = p1.read();
						if ((temp & 0x01) != 0)
						{
							(void)p2.read();
							continue;
						}
					} while ((temp & 0x02) != 0);
					/* Reset! */
					p1.write(0xFE);
				}
			};
		}
		namespace power {
			class APM {
			public:
				static bool Check() {
					regs16_t r;
					r.ax = 0x5300;
					CommanderKernael::core::cpu::interrupts::call16<0x15>(&r);
					char* pm = (char*)(&r.bx);
					return pm[1] == 'P' && pm[0] == 'M';
				}
				static void Connect() {
					regs16_t r;
					r.ax = 0x5301;
					r.bx = 0x0000;
					CommanderKernael::core::cpu::interrupts::call16<0x15>(&r);
				}
				static void shutdown() {
					/*call 16bit realmode interrupt*/
					regs16_t r;
					r.ax = 0x5307;
					r.bx = 0x0001;
					r.cx = 0x0003;
					CommanderKernael::core::cpu::interrupts::call16<0x15>(&r);
					return;
				}
				static void suspend() {
					/*call 16bit realmode interrupt*/
					regs16_t r;
					r.ax = 0x5307;
					r.bx = 0x0001;
					r.cx = 0x0002;
					CommanderKernael::core::cpu::interrupts::call16<0x15>(&r);
					return;
				}
			};
		}
	}
}

#include <Headers/multitasking.hpp>
#include <libc/exceptions.hpp>

using namespace CommanderKernael::multitasking;
using namespace System::Exceptions;

namespace CommanderKernael{
	namespace main{
		extern TaskManager* taskManager;
		void StopTimer();
		void StartTimer();
	}
}
//isr_handler
extern "C" void isr_handler(registers_t r){
	if (CommanderKernael::main::taskManager == 0)
		error::panic(CommanderKernael::core::cpu::exception_messages[(int)(r.int_no)], "Fatal Error", "CPU");
	else{
		ExceptionHandler::Throw(IllegalOperation());
	}
}
extern "C" void page_fault(registers_t r) {
	uint32_t faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
	error::panic("Page Fault at " + string(intToASCII(faulting_address)), "Fatal Error", "Paging System");
}

void* operator new(size_t size) {
	uint8_t* res = (uint8_t*)CommanderKernael::core::memory::allocateMemory(size);
	res[size] = 0;
	return (void*)res;
}

void operator delete(void* obj, unsigned int size) {
	CommanderKernael::core::memory::releaseMemory(obj, size);
}

#endif //__COMMANDERKERNAEL_CORE_CORE_HPP
