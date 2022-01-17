#include <drivers/ustar.h>

#include <std/cstring.h>
#include <std/stdio.h>

#include <lib/memory.h>

uint8_t *ramdisk;
size_t   ramdisk_size;