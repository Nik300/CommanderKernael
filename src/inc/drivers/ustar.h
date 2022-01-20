#pragma once

#include <stdint.h>
#include <stddef.h>
#include <std/stddecl.h>

#define USTAR_MAGIC "ustar"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
enum class ustar_typeflag : char
{
	file = '0',
	hardlink = '1',
	symlink = '2',
	char_device = '3',
	block_device = '4',
	directory = '5',
	fifo = '6'
};
struct __packed ustar_entry
{
	char					 name[100];
	char					 mode[8];
	char					 uid[8];
	char					 gid[8];
	char					 size[12];
	char					 last_modified[12];
	char					 checksum[8];
	ustar_typeflag			 typeflag;
	char					 linkname[100];
	char					 magic[6];
	char					 version[2];
	char					 uname[32];
	char					 gname[32];
	char					 devmajor[8];
	char					 devminor[8];
	char					 prefix[155];
	char					 pad[12];
};
#else
typedef enum 
{
	ustar_typeflag_file = '0',
	ustar_typeflag_hardlink = '1',
	ustar_typeflag_symlink = '2',
	ustar_typeflag_char_device = '3',
	ustar_typeflag_block_device = '4',
	ustar_typeflag_directory = '5',
	ustar_typeflag_fifo = '6'
} ustar_typeflag;
typedef struct __packed ustar_entry
{
	char					 name[100];
	char					 mode[8];
	char					 uid[8];
	char					 gid[8];
	char					 size[12];
	char					 last_modified[12];
	char					 checksum[8];
	ustar_typeflag			 typeflag;
	char					 linkname[100];
	char					 magic[6];
	char					 version[2];
	char					 uname[32];
	char					 gname[32];
	char					 devmajor[8];
	char					 devminor[8];
	char					 prefix[155];
	char					 pad[12];
} ustar_entry;
#endif

// TODO: implement streams and virtual filesystems
// for now just read from a ramdisk
__cdecl uint8_t *ramdisk;
__cdecl size_t ramdisk_size;

__cdecl typedef ustar_entry *TAR_DIR;
__cdecl typedef ustar_entry *TAR_FILE;

__cdecl bool tar_root_validate();
__cdecl size_t tar_calc_size();

__cdecl uint8_t *tar_fopen(const char *path);
__cdecl size_t   tar_ftell(const char *path);

__cdecl void tar_list_all();