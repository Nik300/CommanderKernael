#include <std/stddecl.h>
#include <stdint.h>
#include <stddef.h>
#include <std/int_handler.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

__cdecl void syscall(regs32_t *regs);