#pragma once
#include <stddecl.h>
#include <stdint.h>

#ifndef __cplusplus
typedef struct scall_regs
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
} scall_regs_t;
#else
struct scall_regs_t
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
};
#endif

inline int scall(scall_regs_t *regs)
{
	asm("mov %0, %%eax" : : "m"(regs->eax));
	asm("mov %0, %%ebx" : : "m"(regs->ebx));
	asm("mov %0, %%ecx" : : "m"(regs->ecx));
	asm("mov %0, %%edx" : : "m"(regs->edx));
	asm("int $0x30 \n\
		mov %%eax, %0 \n\
		mov %%ebx, %1 \n\
		mov %%ecx, %2 \n\
		mov %%edx, %3" : "=m"(regs->eax), "=m"(regs->ebx), "=m"(regs->ecx), "=m"(regs->edx));
	return regs->eax;
}
