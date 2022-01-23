#pragma once

#include <stddecl.h>
#include <stdint.h>
#include <stdarg.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <kernel.h>

inline int print(const char *str)
{
	scall_regs_t regs = {
		.eax = 0,
		.ebx = (uint32_t)str,
		.ecx = 0,
		.edx = 0
	};
	return scall(&regs);
}
inline int printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	scall_regs_t regs = {
		.eax = 3,
		.ebx = (uint32_t)fmt,
		.ecx = (uint32_t)&args,
		.edx = 0
	};
	int ret = scall(&regs);
	va_end(args);
	return ret;
}
inline void exit()
{
	scall_regs_t regs = {
		.eax = 1,
		.ebx = 0,
		.ecx = 0,
		.edx = 0
	};
	scall(&regs);
}