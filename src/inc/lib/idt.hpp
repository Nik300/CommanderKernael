#pragma once
#ifndef __cplusplus
#error C++ is required to include this header
#endif

#define SYSCALL_IRQ 96

#include <stddef.h>
#include <stdint.h>

#include <std/int_handler.h>

enum class idt32_gate_t : uint8_t
{
	TASK = 0x5,

	INTERRUPT_16 = 0x6,
	TRAP_16 = 0x7,

	INTERRUPT_32 = 0xE,
	TRAP_32 = 0xF
} __attribute__((packed));
enum class idt32_DPL_t : uint8_t
{
	KERNEL = 0,
	RING1 = 1,
	RING2 = 2,
	USER = 3
};

struct idt32_gate_attribute
{
	idt32_gate_t type 		: 4;
	uint8_t      unused 	: 1;
	uint8_t 	 DPL		: 2;
	uint8_t		 present	: 1;
} __attribute__((packed));

struct idt32_gate
{
	uint16_t 			 offset_low; 	// gate's offset low 16bit
	uint16_t 			 segment; 		// segment selector from GDT (code, data, system)
	
	uint8_t 			 UNUSED;		// unused byte, should always be 0

	idt32_gate_t type 		: 4;
	uint8_t      unused 	: 1;
	uint8_t 	 DPL		: 2;
	bool		 present	: 1;

	uint16_t			 offset_high;	// gate's offset high 16bit;
} __attribute__((packed));

struct idt_register_t{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

const idt32_gate& idt32_set_gate(uint8_t n, uintptr_t handler, uint8_t access_ring = 3, idt32_gate_t gate_type = idt32_gate_t::INTERRUPT_32, uint16_t segment = 0x08);
bool idt32_init();
void enable_interrupts();