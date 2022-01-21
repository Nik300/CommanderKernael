#include <std/cstring.h>
#include <std/stdio.h>

#include <lib/heap.hpp>
#include <lib/memory.h>
#include <lib/serial.h>
#include <lib/process.hpp>

#include <kernel.h>

using namespace System::Memory;

__cdecl void *kend;

Heap::Heap(void* buffer, size_t total_sz, bool paged)
: sz(total_sz-((total_sz/ENTRY_SZ)*sizeof(EntryInfo))), max_entries(sz/ENTRY_SZ), current_entry(0), paged(paged)
{
	if (buffer == nullptr) return;
	InitializeBuffer(buffer);
}

void Heap::InitializeBuffer(void *buffer)
{
	dprintf("[HEAP] Initializing heap buffer at 0x%x\n", buffer);
	info_buffer = (EntryInfo*)buffer;
	if (!paged)
	{
		page_map_addr_sz((uintptr_t)info_buffer, (uintptr_t)mem_align((uintptr_t)info_buffer) + 4*1024*1024, (size_t)mem_align((max_entries*sizeof(EntryInfo))), {present: true, rw: read_write, privilege: supervisor, reserved_1: (0), accessed: false, dirty: false});
	}
	data_buffer = (void*)mem_align((uintptr_t)(buffer + (sizeof(EntryInfo)*max_entries)));
}

void* Heap::AllocEntries(size_t count, uint32_t PID)
{
	int i = 0, ind = 0;
	
	loop:
		int s;
		if (i + count >= max_entries) return nullptr;
		bool found = true;
		for (ind = i, s = 0; s < count; s++)
		{
			if (i+s >= current_entry)
			{
				for (int j = 0; j < count-s; j++, current_entry++)
				{
					if (!paged) page_map_addr_sz((uintptr_t)data_buffer + (s+i+j)*ENTRY_SZ, (uintptr_t)data_buffer + (s+i+j)*ENTRY_SZ, (size_t)ENTRY_SZ, {present: true, rw: read_write, privilege: supervisor, reserved_1: (0), accessed: false, dirty: false});
					info_buffer[s+i+j].used = 0;
					info_buffer[s+i+j].dirty = 0;
				}
				current_entry++;
			}
			if (info_buffer[i+s].used) { found = false; break; }
			if (info_buffer[i+s].dirty) memsetl(data_buffer+(ENTRY_SZ*(i+s)), 0, ENTRY_SZ);
		}
	if (!found) { i++; goto loop; }
	for (int y = ind, i = 0; i < count; y++, i++) { info_buffer[y].used = true; info_buffer[y].dirty = true; info_buffer[y].AllocPID = PID; }
	return data_buffer + (ENTRY_SZ*ind);
}
void Heap::FreeEntry(void *data)
{
	uint32_t ind = ((uint32_t)data - (uint32_t)data_buffer)/ENTRY_SZ;
	if (ind >= 0 && ind < current_entry && info_buffer[ind].used) { info_buffer[ind].used = false; }
}
void Heap::FreeEntries(void *data, size_t count)
{
	for (int i = 0; i < count; i++) { FreeEntry(data+i*ENTRY_SZ); }
}
void Heap::FreeAllProcessEntries(uint32_t PID)
{
	for (int i = 0; i < current_entry; i++)
	{
		if (info_buffer[i].used && info_buffer[i].AllocPID == PID)
		{
			info_buffer[i].used = false;
			dprintf("Free'd entry %d allocated by PID %d\n", i, PID);
		}
	}
}
const EntryInfo &Heap::GetInfo(const void *data) const
{
	uint32_t ind = ((uint32_t)data - (uint32_t)data_buffer)/ENTRY_SZ;
	return info_buffer[ind];
}
EntryInfo *Heap::GetInfoBuffer()
{
	return info_buffer;
}
void *Heap::GetDataBuffer()
{
	return data_buffer;
}
size_t Heap::GetSize() const
{
	return sz;
}
size_t Heap::GetUsedSize() const
{
	size_t s = 0;
	
	for (int i = 0; i < current_entry; i++)
		if (info_buffer[i].used) s += ENTRY_SZ;

	return s;
}
size_t Heap::GetFreeSize() const
{
	return GetSize() - GetUsedSize();
}
int Heap::GetMaxEntries() const
{
	return max_entries;
}

Heap *Heap::CurrentHeap = &System::Kernel::KernelHeap;

Heap *Heap::GetCurrentHeap() { return Heap::CurrentHeap; }
void  Heap::SetCurrentHeap(Heap *heap)
{
	Heap::CurrentHeap = heap;
}
uint32_t Heap::GetLastEntryAddress()
{
	return ((uintptr_t)data_buffer) + (((uint32_t)max_entries)*ENTRY_SZ);
}