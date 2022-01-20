#include <drivers/ustar.h>

#include <std/cstring.h>
#include <std/stddecl.h>
#include <std/stdio.h>

#include <lib/memory.h>
#include <lib/serial.h>

__cdecl bool tar_root_validate()
{
	TAR_DIR root = ((TAR_DIR)ramdisk)+0;

	return strcmp(root->magic, USTAR_MAGIC) == 0;
}
__cdecl size_t tar_calc_size()
{
	uintptr_t entry = (uintptr_t)ramdisk;
	size_t size = 0;

	if (!tar_root_validate()) return 0;

	for (TAR_DIR dir = (TAR_DIR)entry; strcmp(dir->magic, USTAR_MAGIC) == 0; dir = (TAR_DIR)entry)
	{
		size_t sz = stroct(dir->size);
		entry += (((sz + 511) / 512) + 1) * 512;
		size += stroct(dir->size) + sizeof(ustar_entry);
	}

	return ramdisk_size = size;
}
__cdecl uint8_t *tar_fopen(const char *name)
{
	uintptr_t entry = (uintptr_t)ramdisk;

	if (!tar_root_validate()) return 0;

	for (TAR_DIR dir = (TAR_DIR)entry; strcmp(dir->magic, USTAR_MAGIC) == 0; dir = (TAR_DIR)entry)
	{
		if (dir->typeflag == ustar_typeflag::directory)
		{
			entry += sizeof(ustar_entry);
			continue;
		}
		size_t sz = stroct(dir->size);

		if (strcmp(dir->name, name) == 0)
		{
			return (uint8_t*)entry + 512;
		}

		entry += (((sz + 511) / 512) + 1) * 512;
	}

	return 0;
}
__cdecl size_t tar_ftell(const char *path)
{
	uintptr_t entry = (uintptr_t)ramdisk;
	size_t size = 0;

	if (!tar_root_validate()) return 0;

	for (TAR_DIR dir = (TAR_DIR)entry; strcmp(dir->magic, USTAR_MAGIC) == 0; dir = (TAR_DIR)entry)
	{
		size_t sz = stroct(dir->size);

		if (strcmp(dir->name, path) == 0)
		{
			return sz;
		}

		entry += (((sz + 511) / 512) + 1) * 512;
		size += stroct(dir->size);
	}

	return 0;
}
__cdecl void tar_list_all()
{
	using namespace System::IO;

    uintptr_t entry = (uintptr_t)ramdisk;

    if (!tar_root_validate()) return;

    for (TAR_DIR dir = (TAR_DIR)entry; strcmp(dir->magic, USTAR_MAGIC) == 0; dir = (TAR_DIR)entry)
    {
        
        if (dir->typeflag == ustar_typeflag::directory)
        {
            entry += sizeof(ustar_entry);
            continue;
        }
        size_t sz = stroct(dir->size);
        Console::WriteLine(dir->name);

        entry += (((sz + 511) / 512) + 1) * 512;
        
    }
}