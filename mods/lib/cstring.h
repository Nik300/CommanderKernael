#pragma once
#include <dylink.h>

static inline size_t strlen(const char *s)
{
	size_t(*strlen)(const char *s) = (size_t(*)(const char *s))dyload("strlen");

	return strlen(s);
}
static inline char *strcpy(char *dest, const char *src)
{
	char *(*strcpy)(char *dest, const char *src) = (char *(*)(char *dest, const char *src))dyload("strcpy");

	return strcpy(dest, src);
}
static inline char *strncpy(char *dest, const char *src, size_t n)
{
	char *(*strncpy)(char *dest, const char *src, size_t n) = (char *(*)(char *dest, const char *src, size_t n))dyload("strncpy");

	return strncpy(dest, src, n);
}
static inline char *strcat(char *dest, const char *src)
{
	char *(*strcat)(char *dest, const char *src) = (char *(*)(char *dest, const char *src))dyload("strcat");

	return strcat(dest, src);
}
static inline char *strncat(char *dest, const char *src, size_t n)
{
	char *(*strncat)(char *dest, const char *src, size_t n) = (char *(*)(char *dest, const char *src, size_t n))dyload("strncat");

	return strncat(dest, src, n);
}
static inline int strcmp(const char *s1, const char *s2)
{
	int(*strcmp)(const char *s1, const char *s2) = (int(*)(const char *s1, const char *s2))dyload("strcmp");

	return strcmp(s1, s2);
}
static inline int strncmp(const char *s1, const char *s2, size_t n)
{
	int(*strncmp)(const char *s1, const char *s2, size_t n) = (int(*)(const char *s1, const char *s2, size_t n))dyload("strncmp");

	return strncmp(s1, s2, n);
}
static inline char *strchr(const char *s, int c)
{
	char *(*strchr)(const char *s, int c) = (char *(*)(const char *s, int c))dyload("strchr");

	return strchr(s, c);
}
static inline char *strrchr(const char *s, int c)
{
	char *(*strrchr)(const char *s, int c) = (char *(*)(const char *s, int c))dyload("strrchr");

	return strrchr(s, c);
}
static inline char *strstr(const char *haystack, const char *needle)
{
	char *(*strstr)(const char *haystack, const char *needle) = (char *(*)(const char *haystack, const char *needle))dyload("strstr");

	return strstr(haystack, needle);
}
static inline char *strtok(char *s, const char *delim)
{
	char *(*strtok)(char *s, const char *delim) = (char *(*)(char *s, const char *delim))dyload("strtok");

	return strtok(s, delim);
}
static inline char *strrtok(char *s, const char *delim)
{
	char *(*strrtok)(char *s, const char *delim) = (char *(*)(char *s, const char *delim))dyload("strrtok");

	return strrtok(s, delim);
}
static inline int strnum(char *s, int base)
{
	int(*strnum)(char *s, int base) = (int(*)(char *s, int base))dyload("strnum");

	return strnum(s, base);
}
static inline int strdec(char *s)
{
	int(*strdec)(char *s) = (int(*)(char *s))dyload("strdec");

	return strdec(s);
}
static inline int strhex(char *s)
{
	int(*strhex)(char *s) = (int(*)(char *s))dyload("strhex");

	return strhex(s);
}
static inline int stroct(char *s)
{
	int(*stroct)(char *s) = (int(*)(char *s))dyload("stroct");

	return stroct(s);
}
static inline int strbin(char *s)
{
	int(*strbin)(char *s) = (int(*)(char *s))dyload("strbin");

	return strbin(s);
}
static inline char *numstr(int n, int base)
{
	char *(*numstr)(int n, int base) = (char *(*)(int n, int base))dyload("numstr");

	return numstr(n, base);
}
static inline char *decstr(int n)
{
	char *(*decstr)(int n) = (char *(*)(int n))dyload("decstr");

	return decstr(n);
}
static inline char *hexstr(int n)
{
	char *(*hexstr)(int n) = (char *(*)(int n))dyload("hexstr");

	return hexstr(n);
}
static inline char *octstr(int n)
{
	char *(*octstr)(int n) = (char *(*)(int n))dyload("octstr");

	return octstr(n);
}
static inline char *binstr(int n)
{
	char *(*binstr)(int n) = (char *(*)(int n))dyload("binstr");

	return binstr(n);
}
static inline char *numstr_buf(int n, int base, char *buf)
{
	char *(*numstr_buf)(int n, int base, char *buf) = (char *(*)(int n, int base, char *buf))dyload("numstr_buf");

	return numstr_buf(n, base, buf);
}
static inline char *strrev(char *s)
{
	char *(*strrev)(char *s) = (char *(*)(char *s))dyload("strrev");

	return strrev(s);
}