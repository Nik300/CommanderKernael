#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef void (*__module_entry_t)();

bool page_modules();
void run_module(size_t indx);
void *get_module(size_t indx);
size_t get_module_size(size_t indx);

void dyexport(const char* name, const void *addr);
void *dyload(const char* name);

#ifdef __cplusplus
}
#endif