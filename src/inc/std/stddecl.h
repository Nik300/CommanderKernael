#pragma once

#ifdef __cplusplus
#define __cdecl extern "C"
#else
#define __cdecl
#endif

#define __packed __attribute__((packed))
#define __noreturn __attribute__((noreturn))
#define __unused __attribute__((unused))
#define __used __attribute__((used))
#define __weak __attribute__((weak))
#define __noclone __attribute__((noinline))
#define __noclone_fastcall __attribute__((noinline, fastcall))
#define __noclone_regparm(n) __attribute__((noinline, regparm(n)))
#define __section(x) __attribute__((section(#x)))