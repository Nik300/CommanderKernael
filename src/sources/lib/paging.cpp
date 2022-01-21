#include <lib/paging.h>
#include <std/stdio.h>
#include <lib/memory.h>
#include <std/int_handler.h>
#include <std/stddecl.h>
#include <kernel.h>

#include <lib/serial.h>

#include <stddef.h>
#include <stdint.h>

using System::Memory::Heap;

#define TABLE_SZ 4*1024*1024
#define PAGE_SZ  4*1024

#define nullptr 0

__cdecl void *pheap;
__cdecl int pheap_size;

__cdecl void *kend;
__cdecl void *kstart;

__cdecl void *kheap;
__cdecl int kheap_size;

__cdecl page_dir_t *kernel_dir = nullptr;
__cdecl page_dir_t *current_dir = nullptr;
__cdecl uintptr_t   dir_loc = 0;
__cdecl int page_tables_count = 0;

__cdecl void *kernel_pg_end = nullptr;

__cdecl int last_page = 0;

Heap PagesHeap(nullptr, 0);

__cdecl void __page_dir_flush();
__cdecl void __page_set_cr0();

__cdecl void page_switch_dir(page_dir_t *__dir)
{
	//paging_disable();
	current_dir = __dir;
	dir_loc = (uintptr_t)__dir->entries;
	
	__page_dir_flush();
	__page_set_cr0();
}
__cdecl page_dir_t *get_kernel_dir()
{
	return kernel_dir;
}
__cdecl page_dir_t *get_current_dir()
{
	return current_dir;
}

__cdecl uintptr_t mem_align(uintptr_t addr)
{
	uintptr_t out = (uintptr_t)addr;
    out &= 0xFFFFF000;
    if (out < (uintptr_t)addr) { out += 0x1000; }
    return out;
}
__cdecl uintptr_t mem_dealign(uintptr_t addr)
{
	uintptr_t out = (uintptr_t)addr;
	out &= 0x00000FFF;
	return out;
}

__cdecl void page_map_addr(uint32_t phy, uint32_t virt, page_table_entry_t table_entry)
{
	uint32_t id = virt >> 22;
	page_table_t *table = (page_table_t*)PagesHeap.AllocEntries(1);
	dprintf("[paging] mapping 0x%x\n", table);
	if (table == nullptr) { dprintf("[paging] max pages reached!\n"); return; }

	for (uint32_t i = 0; i < 1024; i++, phy+=0x1000)
	{
		table->entries[i].accessed = table_entry.accessed;
		table->entries[i].dirty = table_entry.dirty;

		table->entries[i].present = true;
		table->entries[i].rw = table_entry.rw;
		table->entries[i].privilege = table_entry.privilege;

		table->entries[i].block_addr = phy>>12;
	}

	current_dir->entries[id].present = true;
	current_dir->entries[id].rw = table_entry.rw;
	current_dir->entries[id].privilege = table_entry.privilege;
	current_dir->entries[id].table_addr = (uintptr_t)table >> 12;
	current_dir->entries[id].page_size = pgsz_4KB;
}
__cdecl void page_map_addr_sz(uint32_t phy, uint32_t virt, size_t sz, page_table_entry_t table_entry)
{
	for (uint32_t i = 0; i < sz; i+=4*1024*1024)
	{
		page_map_addr(phy+i, virt+i, table_entry);
	}
}
__cdecl  void page_map_addr_one_pg(uint32_t phy, uint32_t virt, page_table_entry_t table_entry)
{
	uint32_t id = virt >> 22;
	uint32_t table_id = (virt >> 12) & 0x3FF;
	
	page_table_t *table = (page_table_t*)(current_dir->entries[id].table_addr << 12);
	if (table == nullptr) { table = (page_table_t*)PagesHeap.AllocEntries(1); if (table == nullptr) { dprintf("[paging] max entries reached\n"); return; } }

	table->entries[table_id].accessed = table_entry.accessed;
	table->entries[table_id].dirty = table_entry.dirty;

	table->entries[table_id].present = true;
	table->entries[table_id].rw = table_entry.rw;
	table->entries[table_id].privilege = table_entry.privilege;

	table->entries[table_id].block_addr = phy>>12;

	current_dir->entries[id].present = true;
	if (table_entry.rw > current_dir->entries[id].rw) current_dir->entries[id].rw = table_entry.rw;
	if (table_entry.rw > current_dir->entries[id].privilege) current_dir->entries[id].privilege = table_entry.privilege;

	current_dir->entries[id].table_addr = (uintptr_t)table >> 12;
	current_dir->entries[id].page_size = pgsz_4KB;
}
__cdecl void page_map_addr_one_pg_sz(uint32_t phy, uint32_t virt, size_t sz, page_table_entry_t table_entry)
{
	for (uint32_t i = 0; i < sz; i+=0x1000)
	{
		page_map_addr_one_pg(phy+i, virt+i, table_entry);
	}
}

__cdecl void page_map_addr_dir(uint32_t phy, uint32_t virt, page_dir_t *dir, page_table_entry_t table_entry)
{
	uint32_t id = virt >> 22;
	page_table_t *table = (page_table_t*)PagesHeap.AllocEntries(1);
	dprintf("[paging] mapping 0x%x\n", table);
	if (table == nullptr) { dprintf("[paging] max pages reached!\n"); return; }

	for (uint32_t i = 0; i < 1024; i++, phy+=0x1000)
	{
		table->entries[i].accessed = table_entry.accessed;
		table->entries[i].dirty = table_entry.dirty;

		table->entries[i].present = true;
		table->entries[i].rw = table_entry.rw;
		table->entries[i].privilege = table_entry.privilege;

		table->entries[i].block_addr = phy>>12;
	}

	dir->entries[id].present = true;
	dir->entries[id].rw = table_entry.rw;
	dir->entries[id].privilege = table_entry.privilege;
	dir->entries[id].table_addr = (uintptr_t)table >> 12;
	dir->entries[id].page_size = pgsz_4KB;
}
__cdecl void page_map_addr_dir_sz(uint32_t phy, uint32_t virt, page_dir_t *dir, size_t sz, page_table_entry_t table_entry)
{
	for (uint32_t i = 0; i < sz; i+=4*1024*1024)
	{
		page_map_addr_dir(phy+i, virt+i, dir, table_entry);
	}
}
__cdecl void page_map_addr_dir_one_pg(uint32_t phy, uint32_t virt, page_dir_t *dir, page_table_entry_t table_entry)
{
	uint32_t id = virt >> 22;
	uint32_t table_id = (virt >> 12) & 0x3FF;
	
	page_table_t *table = (page_table_t*)(dir->entries[id].table_addr << 12);
	if (table == nullptr) { table = (page_table_t*)PagesHeap.AllocEntries(1); if (table == nullptr) { dprintf("[paging] max entries reached\n"); return; } }

	table->entries[table_id].accessed = table_entry.accessed;
	table->entries[table_id].dirty = table_entry.dirty;

	table->entries[table_id].present = true;
	table->entries[table_id].rw = table_entry.rw;
	table->entries[table_id].privilege = table_entry.privilege;

	table->entries[table_id].block_addr = phy>>12;

	dir->entries[id].present = true;
	if (table_entry.rw > dir->entries[id].rw) dir->entries[id].rw = table_entry.rw;
	if (table_entry.rw > dir->entries[id].privilege) dir->entries[id].privilege = table_entry.privilege;

	dir->entries[id].table_addr = (uintptr_t)table >> 12;
	dir->entries[id].page_size = pgsz_4KB;
}
__cdecl void page_map_addr_dir_one_pg_sz(uint32_t phy, uint32_t virt, page_dir_t *dir, size_t sz, page_table_entry_t table_entry)
{
	for (uint32_t i = 0; i < sz; i+=0x1000)
	{
		page_map_addr_dir_one_pg(phy+i, virt+i, dir, table_entry);
	}
}

__cdecl void page_unmap_addr(uint32_t virt)
{
	uint32_t id = virt >> 22;
	current_dir->entries[id].present = false;
	PagesHeap.FreeEntry((void*)((uintptr_t)current_dir->entries[id].table_addr << 12));
}
__cdecl void page_unmap_addr_sz(uint32_t virt, size_t sz)
{
	for (uint32_t i = 0; i < sz; i+=4*1024*1024, virt+=4*1024*1024)
	{
		page_unmap_addr(virt);
	}
}
__cdecl void page_unmap_addr_one_pg(uint32_t virt)
{
	uint32_t id = virt >> 22;
	uint32_t table_id = (virt >> 12) & 0x3FF;
	page_table_t *table = (page_table_t*)(current_dir->entries[id].table_addr << 12);
	table->entries[table_id].present = false;
}
__cdecl void page_unmap_addr_one_pg_sz(uint32_t virt, size_t sz)
{
	for (uint32_t i = 0; i < sz; i+=0x1000, virt+=0x1000)
	{
		page_unmap_addr_one_pg(virt);
	}
}
__cdecl void page_unmap_addr_dir(uint32_t virt, page_dir_t *dir)
{
	uint32_t id = virt >> 22;
	dir->entries[id].present = false;
	dprintf("[paging] unmapping 0x%x\n", (current_dir->entries[id].table_addr << 12));
	PagesHeap.FreeEntry((void*)((uintptr_t)dir->entries[id].table_addr << 12));
}
__cdecl void page_unmap_addr_dir_sz(uint32_t virt, page_dir_t *dir, size_t sz)
{
	for (uint32_t i = 0; i < sz; i+=4*1024*1024, virt+=4*1024*1024)
	{
		page_unmap_addr_dir(virt, dir);
	}
}
__cdecl void page_unmap_addr_dir_one_pg(uint32_t virt, page_dir_t *dir)
{
	uint32_t id = virt >> 22;
	uint32_t table_id = (virt >> 12) & 0x3FF;
	page_table_t *table = (page_table_t*)(dir->entries[id].table_addr << 12);
	table->entries[table_id].present = false;
}
__cdecl void page_unmap_addr_dir_one_pg_sz(uint32_t virt, page_dir_t *dir, size_t sz)
{
	for (uint32_t i = 0; i < sz; i+=0x1000, virt+=0x1000)
	{
		page_unmap_addr_dir_one_pg(virt, dir);
	}
}

__cdecl void paging_disable()
{
	__asm__ __volatile__("cli");
	int cr0;
	__asm__ __volatile__("mov %%cr0, %0" : "=r" (cr0));
	cr0 &= ~0x80000000;
	__asm__ __volatile__("mov %0, %%cr0" : : "r" (cr0));
}
__cdecl void page_init_kdir()
{
	if (!kernel_dir) paging_init();
	uintptr_t i;
	for (i = 0; i < (uintptr_t)&kend; i+=4*1024*1024)
	{
		page_map_addr_dir(i, i, kernel_dir);
	}
	kernel_pg_end = (void*)i;
	current_dir = kernel_dir;
}
__cdecl page_dir_t *page_create_dir()
{
	page_dir_t *dir = (page_dir_t*)PagesHeap.AllocEntries(sizeof(page_dir_t)/ENTRY_SZ);
	if (dir == nullptr) { dprintf("[paging] max pages reached!\n"); return nullptr; }
	memset(dir, 0, sizeof(page_dir_t));
	return dir;
}
__cdecl void page_init_dir (page_dir_t *dir)
{
	for (int i = 0; i < 1024; i++)
	{
		dir->entries[i].present = false;
		dir->entries[i].rw = access_t::read_write;
		dir->entries[i].privilege = privilege_t::supervisor;
		dir->entries[i].page_size = page_size_t::pgsz_4KB;
		dir->entries[i].dirty = false;
		dir->entries[i].accessed = false;

		dir->entries[i].table_addr = 0;
	}
	for (uintptr_t i = 0, ind = 0; i < (uintptr_t)&kend; i+=4*1024*1024, ind++)
	{
		dir->entries[ind] = kernel_dir->entries[ind];
	}

	dprintf("[paging] dir 0x%x initialized.\n", dir);
}
__cdecl void page_destroy_dir(page_dir_t *dir)
{
	for (int i = 0; i < 1024; i++)
	{
		if (dir->entries[i].present)
		{
			PagesHeap.FreeEntry((void*)((uintptr_t)dir->entries[i].table_addr << 12));
		}
	}
	PagesHeap.FreeEntries((void*)dir, sizeof(page_dir_t)/ENTRY_SZ);
	dprintf("[paging] dir 0x%x destroyed.\n", dir);
}
__cdecl void paging_init()
{
	PagesHeap = Heap(&pheap, pheap_size, true);
	kernel_dir = (page_dir_t*)PagesHeap.AllocEntries(sizeof(page_dir_t)/ENTRY_SZ);
	for (int i = 0; i < 1024; i++)
	{
		kernel_dir->entries[i].present = false;
		kernel_dir->entries[i].rw = access_t::read_write;
		kernel_dir->entries[i].privilege = privilege_t::supervisor;
		kernel_dir->entries[i].page_size = page_size_t::pgsz_4KB;
		kernel_dir->entries[i].dirty = false;
		kernel_dir->entries[i].accessed = false;

		kernel_dir->entries[i].table_addr = 0;
	}
}
__cdecl void paging_enable()
{
	page_switch_dir(current_dir);
	__asm__ __volatile__("sti");
}