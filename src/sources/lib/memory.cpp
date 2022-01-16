#include <kernel.h>

#include <std/stdio.h>

#include <lib/memory.h>
#include <lib/heap.hpp>
#include <lib/serial.h>

#define MAGIC 0xC0DE

namespace System::Memory
{
	uint32_t entry_ind = 0;
	uint32_t entry_off = 0;
	void *start_entry = nullptr;
}

System::Memory::DataEntry *shrink_entry(System::Memory::DataEntry *entry, uint16_t final_sz)
{
	using namespace System::IO;
	using namespace System::Memory;

	if (final_sz > entry->sz) { return nullptr; }
	else if (entry->sz == final_sz) return entry;
	
	DataEntry *new_entry = (DataEntry*)((uintptr_t)entry + sizeof(DataEntry) + final_sz);
	new_entry->sz = entry->sz - final_sz - sizeof(DataEntry);
	new_entry->state_flags = {
		.used = false,
		.dirty = true,
		.collect = false
	};

	entry->sz = final_sz;
	return entry;
}
System::Memory::DataEntry *combine_entries(System::Memory::DataEntry *from, System::Memory::DataEntry *to)
{
	using namespace System::Memory;
	using namespace System::IO;
	if (to < from) { return nullptr; }
	else if (to == from) return from;

	uint32_t sz = 0;
	for (uintptr_t i = (uintptr_t)from; i < (uintptr_t)to+sizeof(DataEntry);) { sz += ((DataEntry*)i)->sz; i += ((DataEntry*)i)->sz + sizeof(DataEntry); }

	from->sz = sz;
	return from;
}

extern "C" void *malloc(size_t sz)
{
	using namespace System::Memory;
	using namespace System::IO;
	if (!start_entry)
	{
		start_entry = Heap::GetCurrentHeap()->AllocEntries(1);
	}

	uint8_t *current = (uint8_t*)start_entry;
	DataEntry *result = nullptr;

	int i;
	for (i = 0; ;)
	{
		DataEntry *entry = (DataEntry*)(current + i);
		if (entry->magic != MAGIC)
		{
			entry->sz = sz;

			entry->magic = MAGIC;

			entry->state_flags.used = true;
			entry->state_flags.dirty = true;
			entry->state_flags.collect = true;
			result = entry;
			break;
		}
		else if (!entry->state_flags.used && entry->sz >= sz)
		{
			result = shrink_entry(entry, sz);
			break;
		}
		else if (!entry->state_flags.used && entry->sz < sz)
		{
			uintptr_t to = ((uintptr_t)entry + entry->sz + sizeof(DataEntry));
			size_t size = entry->sz;

			if (((DataEntry*)to)->magic != MAGIC)
			{
				((DataEntry*)to)->sz = sz - size;
				((DataEntry*)to)->magic = MAGIC;
				((DataEntry*)to)->state_flags.used = true;
				((DataEntry*)to)->state_flags.dirty = true;
				((DataEntry*)to)->state_flags.collect = true;
				
				result = combine_entries(entry, ((DataEntry*)to));
				break;
			}
			else if (!((DataEntry*)to)->state_flags.used && ((DataEntry*)to)->sz >= sz - size)
			{
				result = combine_entries(entry, shrink_entry(((DataEntry*)to), sz - size));
				break;
			}

			for (; !((DataEntry*)to)->state_flags.used && size < sz && ((DataEntry*)to)->magic == MAGIC;) { size += ((DataEntry*)to)->sz; to += ((DataEntry*)to)->sz + sizeof(DataEntry); }
			if (size >= sz)
			{
				result = shrink_entry(combine_entries(entry, ((DataEntry*)to)), sz);
				break;
			}
		}

		i += sizeof(DataEntry) + entry->sz;
	}
	if (!result) { return nullptr; }

	if (i / ENTRY_SZ > entry_ind) { Heap::GetCurrentHeap()->AllocEntries((i/ENTRY_SZ) - entry_ind); entry_ind = i / ENTRY_SZ; }

	result->state_flags.used = true;
	result->state_flags.dirty = true;
	result->state_flags.collect = true;

	dprintf("[SYSTEM] Allocated %d bytes at 0x%x\n", sz, result);

	return (void*)(((uintptr_t)result) + sizeof(DataEntry));
}
extern "C" void *calloc(size_t sz)
{
	using namespace System::Memory;
	void *dat = malloc(sz);

	DataEntry *info = (DataEntry*)((uintptr_t)dat-sizeof(DataEntry));
	if (!info->state_flags.dirty) { return dat; }

	if (sz % 4 == 0) memsetl(dat, 0, sz);
	else if (sz % 2 == 0) memsetw(dat, 0, sz);
	else memset(dat, 0, sz);
	return dat;
}
extern "C" void *realloc(void *buff, size_t sz)
{
	if (!buff) return calloc(sz);
	void *result = malloc(sz);

	if (sz % 4 == 0) memcpyl(result, buff, sz);
	else if (sz % 2 == 0) memcpyw(result, buff, sz);
	else memcpy(result, buff, sz);
	free(buff);
	return result;
}

extern "C" void free(void *buff)
{
	using namespace System::Memory;
	if (!start_entry) return;

	DataEntry *info = (DataEntry*)((uintptr_t)buff-sizeof(DataEntry));

	if (info->magic != MAGIC) { dprintf("[SYSTEM] Free: magic %x is not valid\n", info->magic); return; }

	info->state_flags.used = false;
	info->state_flags.collect = false;

	dprintf("[SYSTEM] Freed %d bytes at 0x%x\n", info->sz, info);
}
extern "C" void cfree(void *buff)
{
	using namespace System::Memory;
	if (!start_entry) return;

	DataEntry *info = (DataEntry*)((uintptr_t)buff-sizeof(DataEntry));

	if (!info->magic) return;

	info->state_flags.used = false;
	info->state_flags.collect = false;

	if (info->sz % 4) memsetl(buff, 0, info->sz);
	else if (info->sz % 2) memsetw(buff, 0, info->sz);
	else memset(buff, 0, info->sz);

	dprintf("[SYSTEM] Freed %d bytes at 0x%x\n", info->sz, info);
}

__cdecl void *memcpy(void *dest, const void *src, size_t count)
{
	if (count % 4 == 0) memcpyl(dest, src, count);
	else if (count % 2 == 0) memcpyw(dest, src, count);
	else memcpy(dest, src, count);
	return dest;
}