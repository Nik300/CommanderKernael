#pragma once

#include <stddecl.h>
#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <kernel.h>

int print(const char *str)
{
	scall_regs_t regs = {
		.eax = 0,
		.ebx = (uint32_t)str,
		.ecx = 0,
		.edx = 0
	};
	return scall(regs);
}