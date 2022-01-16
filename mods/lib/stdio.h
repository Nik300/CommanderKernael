#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <dylink.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

static inline int printf(const char* format, ...)
{
	// get printv function
	int(*printv)(const char *format, va_list args) = (int(*)(const char *, va_list))dyload("printv");

	va_list args;
	va_start(args, format);
	int ret = printv(format, args);
	va_end(args);
	return ret;
}
static inline int print(const char* text)
{
	// get print function
	int(*print)(const char *text) = (int(*)(const char *))dyload("print");

	return print(text);
}
static inline int printv(const char* format, va_list args)
{
	// get printv function
	int(*printv)(const char *format, va_list args) = (int(*)(const char *, va_list))dyload("printv");

	return printv(format, args);
}