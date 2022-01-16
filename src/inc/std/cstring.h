#pragma once
#include <stddef.h>
#include <stdint.h>

#include <lib/memory.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int strlen(const char* str)
{
	int len = 0;
	while (str[len] != '\0') len++;
	return len;
}
static inline int numlen(uint32_t num)
{
	int len = 0;
	while (num > 0)
	{
		num /= 10;
		len++;
	}
	return len;
}

static inline int strcmp(const char* str1, const char* str2)
{
	int i = 0;
	while (str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] != str2[i]) return str1[i] - str2[i];
		i++;
	}
	return str1[i] - str2[i];
}
static inline int strncmp(const char* str1, const char* str2, size_t n)
{
	int i = 0;
	while (str1[i] != '\0' && str2[i] != '\0' && i < n)
	{
		if (str1[i] != str2[i]) return str1[i] - str2[i];
		i++;
	}
	return str1[i] - str2[i];
}
static inline char* strcpy(char* dest, const char* src)
{
	int i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return dest;
}
static inline char* strncpy(char* dest, const char* src, size_t n)
{
	int i = 0;
	while (src[i] != '\0' && i < n)
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return dest;
}
static inline char* strcat(char* &dest, const char* src)
{
	dest = (char*)realloc(dest, strlen(dest) + strlen(src) + 1);
	int i = 0;
	while (dest[i] != '\0') i++;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return dest;
}
static inline char* strncat(char* &dest, const char* src, size_t n)
{
	dest = (char*)realloc(dest, strlen(dest) + n + 1);
	int i = 0;
	while (dest[i] != '\0') i++;
	while (src[i] != '\0' && i < n)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return dest;
}
static inline char* strchr(const char* str, int c)
{
	int i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == c) return (char*)str + i;
		i++;
	}
	return nullptr;
}
static inline char* strrchr(const char* str, int c)
{
	int i = strlen(str) - 1;
	while (i >= 0)
	{
		if (str[i] == c) return (char*)str + i;
		i--;
	}
	return nullptr;
}
static inline char* strstr(const char* str, const char* substr)
{
	int i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == substr[0])
		{
			int j = 0;
			while (str[i + j] != '\0' && substr[j] != '\0')
			{
				if (str[i + j] != substr[j]) break;
				j++;
			}
			if (substr[j] == '\0') return (char*)str + i;
		}
		i++;
	}
	return nullptr;
}
static inline char* strpbrk(const char* str, const char* accept)
{
	int i = 0;
	while (str[i] != '\0')
	{
		int j = 0;
		while (accept[j] != '\0')
		{
			if (str[i] == accept[j]) return (char*)str + i;
			j++;
		}
		i++;
	}
	return nullptr;
}
static inline char* strtok(char* &str, const char* delim)
{
	if (str == nullptr)
	{
		str = (char*)malloc(1);
		str[0] = '\0';
	}
	char* token = strpbrk(str, delim);
	if (token == nullptr)
	{
		char* ret = str;
		str = nullptr;
		return ret;
	}
	*token = '\0';
	str = token + 1;
	return token;
}
static inline char* strtok_r(char* &str, const char* delim, char* &saveptr)
{
	if (str == nullptr)
	{
		str = (char*)malloc(1);
		str[0] = '\0';
	}
	if (saveptr == nullptr) saveptr = str;
	char* token = strpbrk(saveptr, delim);
	if (token == nullptr)
	{
		char* ret = saveptr;
		saveptr = nullptr;
		return ret;
	}
	*token = '\0';
	saveptr = token + 1;
	return token;
}
static inline char* strsep(char* &str, const char* delim)
{
	if (str == nullptr)
	{
		str = (char*)malloc(1);
		str[0] = '\0';
	}
	char* token = strpbrk(str, delim);
	if (token == nullptr)
	{
		char* ret = str;
		str = nullptr;
		return ret;
	}
	*token = '\0';
	str = token + 1;
	return token;
}
static inline char* strdelc(char* &str, int c)
{
	char *result = (char*)malloc(strlen(str) - 1);
	int i = 0;
	int j = 0;
	while (str[i] != '\0')
	{
		if (str[i] != c)
		{
			result[j] = str[i];
			j++;
		}
		i++;
	}
	result[j] = '\0';
	str = result;
	return str;
}
static inline char* strdelc_r(char* &str, int c)
{
	char *result = (char*)malloc(strlen(str) - 1);
	int i = 0;
	int j = 0;
	while (str[i] != '\0')
	{
		if (str[i] != c)
		{
			result[j] = str[i];
			j++;
		}
		i++;
	}
	result[j] = '\0';
	str = result;
	return str;
}
static inline char* strdels(char* &str, const char* del)
{
	char *result = (char*)malloc(strlen(str) - strlen(del));
	int i = 0;
	int j = 0;
	while (str[i] != '\0')
	{
		int k = 0;
		while (del[k] != '\0')
		{
			if (str[i] == del[k]) break;
			k++;
		}
		if (del[k] == '\0')
		{
			result[j] = str[i];
			j++;
		}
		i++;
	}
	result[j] = '\0';
	str = result;
	return str;
}
static inline char* strdels_r(char* &str, const char* del)
{
	char *result = (char*)malloc(strlen(str) - strlen(del));
	int i = 0;
	int j = 0;
	while (str[i] != '\0')
	{
		int k = 0;
		while (del[k] != '\0')
		{
			if (str[i] == del[k]) break;
			k++;
		}
		if (del[k] == '\0')
		{
			result[j] = str[i];
			j++;
		}
		i++;
	}
	result[j] = '\0';
	str = result;
	return str;
}
static inline char* strdel(char* &str, const char* del)
{
	return strdels(str, del);
}
static inline char* strdel_r(char* &str, const char* del)
{
	return strdels_r(str, del);
}
static inline char* strrev(char* str)
{
	int i = 0;
	int j = strlen(str) - 1;
	while (i < j)
	{
		char c = str[i];
		str[i] = str[j];
		str[j] = c;
		i++;
		j--;
	}
	return str;
}
static inline int strnum(const char* str, int base)
{
	int ret = 0;
	int i = 0;
	while (str[i] != '\0')
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			ret = ret * base + str[i] - '0';
		}
		else if (str[i] >= 'a' && str[i] <= 'z')
		{
			ret = ret * base + str[i] - 'a' + 10;
		}
		else if (str[i] >= 'A' && str[i] <= 'Z')
		{
			ret = ret * base + str[i] - 'A' + 10;
		}
		i++;
	}
	return ret;
}
static inline char* numstr(uint32_t num, int base)
{
	if (num == 0) 
	{
		char *res = (char*)malloc(2);
		res[0] = '0';
		res[1] = '\0';
		return res;
	}
	char* str = (char*)malloc(numlen(num)+1);
	int i = 0;
	while (num > 0)
	{
		int r = num % base;
		if (r >= 10)
		{
			str[i] = r - 10 + 'a';
		}
		else
		{
			str[i] = r + '0';
		}
		num = num / base;
		i++;
	}
	str[i] = '\0';
	strrev(str);
	return str;
}
static inline char* numstr_buf(uint32_t num, int base, char* buf)
{
	if (num == 0) 
	{
		buf[0] = '0';
		buf[1] = '\0';
		return buf;
	}
	int i = 0;
	while (num > 0)
	{
		int r = num % base;
		if (r >= 10)
		{
			buf[i] = r - 10 + 'a';
		}
		else
		{
			buf[i] = r + '0';
		}
		num = num / base;
		i++;
	}
	buf[i] = '\0';
	strrev(buf);
	return buf;
}
static inline int strdec(const char* str)
{
	return strnum(str, 10);
}
static inline int strhex(const char* str)
{
	return strnum(str, 16);
}
static inline int strbin(const char* str)
{
	return strnum(str, 2);
}
static inline int stroct(const char* str)
{
	return strnum(str, 8);
}
static inline char* decstr(int num)
{
	return numstr(num, 10);
}
static inline char* hexstr(int num)
{
	return numstr(num, 16);
}
static inline char* binstr(int num)
{
	return numstr(num, 2);
}
static inline char* octstr(int num)
{
	return numstr(num, 8);
}

#ifdef __cplusplus
}
#endif