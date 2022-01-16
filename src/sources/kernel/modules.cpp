#include <kernel.h>
#include <init/modules.h>

#include <lib/paging.h>
#include <lib/memory.h>
#include <lib/serial.h>

#include <std/stdio.h>
#include <std/cstring.h>
#include <std/logging.h>
#include <std/stddecl.h>

static void* dy_exports[256] = {0};
static char* dy_names[256] = {0};
static int dy_count = 0;
static uintptr_t current_module = 0;

__cdecl bool page_modules()
{
	int mods_count = multiboot_data->mods_count;
	if (mods_count == 0)
	{
		return false;
	}

	uintptr_t *mods = (uintptr_t*)multiboot_data->mods_addr;
	void *first_mod = (void*)mods[0];
	void *last_mod = (void*)mods[mods_count - 1];

	dprintf("%x, %x\n", first_mod, last_mod);

	for (uintptr_t i = (uintptr_t)first_mod; i <= mem_align((void*)last_mod) + 0x1000; i += 0x1000)
	{
		page_map_addr(i, i, { present: true, rw: read_write, privilege: supervisor, 0, accessed: false, dirty: true });
	}
	return true;
}
__cdecl void run_module(size_t indx)
{
	uintptr_t *mods = (uintptr_t*)multiboot_data->mods_addr;
	void *mod = (void*)mods[indx];
	__module_entry_t entry = (__module_entry_t)mod;
	current_module = (uintptr_t)mod;
	entry();
	current_module = 0;
}
__cdecl void *get_module(size_t indx)
{
	uintptr_t *mods = (uintptr_t*)multiboot_data->mods_addr;
	return (void*)mods[indx];
}
__cdecl size_t get_module_size(size_t indx)
{
	uintptr_t *mods = (uintptr_t*)multiboot_data->mods_addr;
	return (size_t)mods[indx + 1] - (size_t)mods[indx];
}

__cdecl void dyexport(const char* name, const void *addr)
{
	dy_names[dy_count] = (char*)name;

	dy_exports[dy_count] = (void*)addr;
	dy_count++;
}
__cdecl void *dyload(const char *name)
{
	for (int i = 0; i < dy_count; i++)
	{
		if (strcmp(name, dy_names[i]) == 0)
		{
			return dy_exports[i];
		}
	}
	return 0;
}