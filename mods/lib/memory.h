#pragma once

#include <stddecl.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <kernel.h>

#define PAGE_SZ 4096

inline void *palloc(size_t count)
{
	scall_regs_t regs = {
		.eax = 2,
		.ebx = (uint32_t)count,
		.ecx = 0,
		.edx = 0
	};
	int result = scall(&regs);
	if (result == 0) return (void*)regs.ecx;
	return NULL;
}