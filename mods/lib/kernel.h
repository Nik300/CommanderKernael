#pragma once
#include <dylink.h>

typedef struct regs32
{
	uint32_t			 ds;
	uint32_t			 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	
	union {
		uint32_t		 interrupt_number;
		uint32_t		 trap_number;
	} __attribute__((packed)) identifier;

	uint32_t			 eip, cs, eflags, useresp, ss;
} __attribute__((packed)) regs32_t;

typedef void (*handler_t)(regs32_t*);

static inline void regirq(uint8_t irq, handler_t handler)
{
	void(*regirq)(uint8_t irq, handler_t handler) = (void(*)(uint8_t irq, handler_t handler))dyload("regirq");
	regirq(irq, handler);
}