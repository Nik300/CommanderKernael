#pragma once

#include <kernel.h>

#include <lib/ports.h>
#include <lib/memory.h>
#include <lib/paging.h>

#include <std/stdio.h>
#include <std/stddecl.h>

#ifdef __cplusplus
namespace System::IO::Serial
{
	enum class SerialPort : uint16_t
	{
		COM1 = 0x3F8,
		COM2 = 0x2F8,
		COM3 = 0x3E8,
		COM4 = 0x2E8
	};
	void Init(SerialPort port);
	
	void Write(const char *str, ...);
	void WriteLine(const char *str, ...);

	void Disable();
}

__cdecl {
#else
#include <stdbool.h>
#endif

void dprint(const char *str);
void dprintf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif