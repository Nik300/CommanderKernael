#pragma once

#include <stdint.h>
#include <stddef.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef void (*linker_function)(...);

static inline void *dyload(const char *name)
{
	
}
static inline void dyexport(const char *name, const void *addr)
{
	void (*dyexport)(const char *name, const void *addr) = (void (*)(const char *name, const void *addr))dyload("dyexport");
	dyexport(name, addr);
}