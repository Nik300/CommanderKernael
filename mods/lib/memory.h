#pragma once

#include <stdint.h>
#include <stddef.h>
#include <dylink.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

static inline void *memset(void *s, int c, size_t n)
{
	void *(*memset)(void *s, int c, size_t n) = (void *(*)(void *s, int c, size_t n))dyload("memset");

	return memset(s, c, n);
}
static inline void *memcpy(void *dest, const void *src, size_t n)
{
	void *(*memcpy)(void *dest, const void *src, size_t n) = (void *(*)(void *dest, const void *src, size_t n))dyload("memcpy");

	return memcpy(dest, src, n);
}
static inline bool memcmp(const void *s1, const void *s2, size_t n)
{
	bool (*memcmp)(const void *s1, const void *s2, size_t n) = (int(*)(const void *s1, const void *s2, size_t n))dyload("memcmp");

	return memcmp(s1, s2, n);
}
static inline void *memmove(void *dest, const void *src, size_t n)
{
	void *(*memmove)(void *dest, const void *src, size_t n) = (void *(*)(void *dest, const void *src, size_t n))dyload("memmove");

	return memmove(dest, src, n);
}