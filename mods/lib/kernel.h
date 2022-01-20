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

inline int scall(scall_regs_t regs)
{
	asm volatile ("int $0x30" : : "a"(regs.eax), "b"(regs.ebx), "c"(regs.ecx), "d"(regs.edx));
	return regs.eax;
}
