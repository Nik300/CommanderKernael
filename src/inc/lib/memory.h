#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif
static inline void* memcpyl(void* dest, const void* src, size_t size)
{
	size_t l = size/4;
	asm volatile("cld;rep movsl" : "+D"(dest), "+S"(src), "+c"(l) : : "memory");
	return dest;
}
static inline void* memcpyw(void* dest, const void* src, size_t size)
{
	size_t l = size/2;
	asm volatile("cld;rep movsw" : "+D"(dest), "+S"(src), "+c"(l) : : "memory");
	return dest;
}
static inline void* memcpyb(void* dest, const void* src, size_t size)
{
	asm volatile("cld;rep movsb" : "+D"(dest), "+S"(src), "+c"(size) : : "memory");
	return dest;
}
void* memcpy(void* dest, const void* src, size_t size);

static inline void* memsetl(void* dest, uint32_t data, size_t size)
{
	size_t l = size/4;
	asm volatile("cld;rep stosl" : "+D"(dest), "+c"(l) : "a"(data) : "memory");
	return dest;
}
static inline void* memsetw(void* dest, uint16_t data, size_t size)
{
	size_t l = size/2;
	asm volatile("cld;rep stosw" : "+D"(dest), "+c"(l) : "a"(data) : "memory");
	return dest;
}
static inline void* memsetb(void* dest, uint8_t data, size_t size)
{
	asm volatile("cld;rep stosb" : "+D"(dest), "+c"(size) : "a"(data) : "memory");
	return dest;
}
static inline void* memset(void* dest, uint8_t data, size_t size)
{
	if (size % 4 == 0) return memsetl(dest, data, size);
	else if (size % 2 == 0) return memsetw(dest, data, size);
	else return memsetb(dest, data, size);
}

static inline bool memcmpl(const void* src, const void* dest, size_t size)
{
	size_t l = size/4;
	asm volatile("cld;rep cmpsl" : "+S"(src), "+D"(dest), "+c"(l) : : "memory");
	bool result;
	asm volatile("setz %0" : "=r"(result));
	return result;
}
static inline bool memcmpw(const void* src, const void* dest, size_t size)
{
	size_t l = size/2;
	asm volatile("cld;rep cmpsw" : "+S"(src), "+D"(dest), "+c"(l) : : "memory");
	bool result;
	asm volatile("setz %0" : "=r"(result));
	return result;
}
static inline bool memcmpb(const void* src, const void* dest, size_t size)
{
	asm volatile("cld;rep cmpsb" : "+S"(src), "+D"(dest), "+c"(size) : : "memory");
	bool result;
	asm volatile("setz %0" : "=r"(result));
	return result;
}
static inline bool memcmp(const void* src, const void* dest, size_t size)
{
	if (size % 4 == 0) return memcmpl(src, dest, size);
	else if (size % 2 == 0) return memcmpw(src, dest, size);
	else return memcmpb(src, dest, size);
}

#ifdef __cplusplus
static inline void* memmovl(void* dest, void* src, size_t size, int c = 0)
{
	memcpyl(dest, src, size);
	memsetl(src, c, size);
	return dest;
}
static inline void* memmovw(void* dest, void* src, size_t size, int c = 0)
{
	memcpyw(dest, src, size);
	memsetw(src, c, size);
	return dest;
}
static inline void* memmovb(void* dest, void* src, size_t size, int c = 0)
{
	memcpy(dest, src, size);
	memset(src, c, size);
	return dest;
}
static inline void* memmov(void* dest, void* src, size_t size, int c = 0)
{
	if (size % 4 == 0) return memmovl(dest, src, size, c);
	else if (size % 2 == 0) return memmovw(dest, src, size, c);
	else return memmovb(dest, src, size, c);
}
#else
static inline void* memmovl(void* dest, void* src, size_t size, int c)
{
	memcpyl(dest, src, size);
	memsetl(src, c, size);
	return dest;
}
static inline void* memmovw(void* dest, void* src, size_t size, int c)
{
	memcpyw(dest, src, size);
	memsetw(src, c, size);
	return dest;
}
static inline void* memmovb(void* dest, void* src, size_t size, int c)
{
	memcpy(dest, src, size);
	memset(src, c, size);
	return dest;
}
static inline void* memmov(void* dest, void* src, size_t size, int c)
{
	if (size % 4 == 0) return memmovl(dest, src, size, c);
	else if (size % 2 == 0) return memmovw(dest, src, size, c);
	else return memmovb(dest, src, size, c);
}
#endif

void *malloc(size_t size);
void *calloc(size_t size);
void *realloc(void* data, size_t size);

void  free(void *data);
void  cfree(void *data);
#ifdef __cplusplus
}
inline void *operator new(size_t sz)
{
	return calloc(sz);
}
inline void *operator new[](size_t sz)
{
	return calloc(sz);
}

inline void operator delete(void *data, size_t _)
{
	cfree(data);
}
inline void operator delete(void *buf)
{
	void **data = reinterpret_cast<void**>(buf);
	for (int i = 0; data[i]; i++) free(data[i]);
	delete buf;
}
inline void operator delete[](void *buf)
{
	void **data = reinterpret_cast<void**>(buf);
	for (int i = 0; data[i]; i++) free(data[i]);
	delete buf;
}
#endif