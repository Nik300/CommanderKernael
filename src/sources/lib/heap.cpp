#include <std/cstring.h>
#include <std/stdio.h>

#include <lib/heap.hpp>
#include <lib/memory.h>
#include <lib/serial.h>
#include <lib/process.hpp>

#include <kernel.h>

using namespace System::Memory;

Heap::Heap(void* buffer, size_t total_sz)
: sz(total_sz-((total_sz/ENTRY_SZ)*sizeof(EntryInfo))), max_entries(sz/ENTRY_SZ)
{
	if (buffer == nullptr) return;
	InitializeBuffer(buffer);
}

void Heap::InitializeBuffer(void *buffer)
{
	info_buffer = (EntryInfo*)buffer;
	for(int i = 0; i < max_entries; i++)
		info_buffer[i] = {
			.used = false,
			.dirty = true
		};
	data_buffer = (void*)mem_align((void*)(buffer + (sizeof(EntryInfo)*max_entries)));
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
			if (info_buffer[i].used) { found = false; break; }
			if (info_buffer[i].dirty) memsetl(data_buffer+(ENTRY_SZ*i), 0, ENTRY_SZ);
		}
	if (!found) { i++; goto loop; }
	for (int y = ind, i = 0; i < count; y++, i++) { info_buffer[y].used = true; info_buffer[y].dirty = true; info_buffer[y].AllocPID = PID; }
	return data_buffer + (ENTRY_SZ*ind);
}
void Heap::FreeEntry(void *data)
{
	uint32_t ind = ((uint32_t)data - (uint32_t)data_buffer)/ENTRY_SZ;
	if (ind >= 0 && ind < max_entries && info_buffer[ind].used) { info_buffer[ind].used = false; }
}
void Heap::FreeEntries(void *data, size_t count)
{
	uint32_t ind = ((uint32_t)data - (uint32_t)data_buffer)/ENTRY_SZ;
	if (!(ind >= 0 && ind < max_entries && info_buffer[ind].used)) return;
	for (; ind < count; ind++) info_buffer[ind].used = false;
}
void Heap::FreeAllProcessEntries(uint32_t PID)
{
	for (int i = 0; i < max_entries; i++)
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
	
	for (int i = 0; i < max_entries; i++)
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