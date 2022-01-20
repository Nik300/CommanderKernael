#pragma once
#ifndef __cplusplus
#error C++ is required for this library
#endif

#include <stdint.h>
#include <stddef.h>

#define ENTRY_SZ 4096

namespace System::Memory
{
	struct EntryInfo
	{
		uint8_t used  : 1;
		uint8_t dirty : 1;

		uint32_t AllocPID: 30;
	}__attribute__((packed));
	struct DataEntry
	{
		uint16_t magic;
		uint32_t sz;
		struct {
			bool used    : 1;
			bool dirty   : 1;
			bool collect : 1;
		} __attribute__((packed)) state_flags;
	}__attribute__((packed));

	class Heap final
	{
	private:
		size_t sz;
		int max_entries;
		EntryInfo *info_buffer;
		void *data_buffer;
		int	  current_entry;
		bool  paged;
	private:
		static Heap *CurrentHeap;
	private:
		void InitializeBuffer(void *buffer);

	public:
		static Heap *GetCurrentHeap();
		static void  SetCurrentHeap(Heap *heap);
	public:
		void *AllocEntries(size_t count, uint32_t PID = 0);
		void FreeEntry(void *entry);
		void FreeEntries(void *entry, size_t count);
		void FreeAllProcessEntries(uint32_t PID);
	public:
		const EntryInfo &GetInfo(const void *entry) const;
		EntryInfo* GetInfoBuffer();
		void* 	   GetDataBuffer();
		uint32_t   GetLastEntryAddress();
		size_t	   GetSize() const;
		size_t	   GetUsedSize() const;
		size_t	   GetFreeSize() const;
		int		   GetMaxEntries() const;
	public:
		Heap(void* buffer, size_t total_sz, bool paged = true);
	};
}