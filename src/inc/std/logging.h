#pragma once
#include <std/stdio.h>

#define MODULE(name) char *__MODULE_NAME__ = name;
#define LOGF(format, ...) printf("[%s] %s\n", __MODULE_NAME__, format, __VA_ARGS__)
#define LOG(format) printf("[%s] %s\n", __MODULE_NAME__, format)