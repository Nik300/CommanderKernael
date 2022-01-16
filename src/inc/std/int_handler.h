#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef struct regs32
{
	uint32_t			 ds;
	uint32_t			 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	
	union {
		uint32_t		 interrupt_number;
		uint32_t		 trap_number;
	} identifier;

	uint32_t			 eip, cs, eflags, useresp, ss;
} regs32_t;

typedef void (*handler_t)(regs32_t*);
extern handler_t *handler_table;

extern void register_int_handler(uint8_t irq, handler_t handler);

#ifdef __cplusplus
}
#endif