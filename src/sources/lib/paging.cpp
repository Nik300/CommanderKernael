#include <lib/paging.h>
#include <std/stdio.h>
#include <lib/memory.h>
#include <std/int_handler.h>
#include <std/stddecl.h>
#include <kernel.h>

#include <lib/serial.h>

#include <stddef.h>
#include <stdint.h>

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

__cdecl int last_page = 0;

__cdecl void __page_dir_flush();
__cdecl void __page_set_cr0();

__cdecl void page_switch_dir(page_dir_t *__dir)
{
	if (current_dir == __dir) return;
	paging_disable();
	current_dir = __dir;


	dir_loc = (uintptr_t)__dir->phy_addr;
	
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

__cdecl uintptr_t mem_align(void *addr)
{
	uintptr_t out = (uintptr_t)addr;
    out &= 0xFFFFF000;
    if (out < (uintptr_t)addr) { out += 0x1000; }
    return out;
}
__cdecl uintptr_t mem_dealign(void *addr)
{
	uintptr_t out = (uintptr_t)addr;
	out &= 0x00000FFF;
	return out;
}

__cdecl void page_dir_init()
{
	kernel_dir = (page_dir_t *)&pheap;
	current_dir = kernel_dir;
	dir_loc = (uintptr_t)&kernel_dir->entries;
	last_page = 0;
	page_tables_count = 0;

	kernel_dir->phy_addr = (uint32_t)&(kernel_dir->entries);

	for (int i = 0; i < 1024; i++)
	{
		uintptr_t table_addr = (uintptr_t)(&kernel_dir->tables_phy[i]);
		kernel_dir->entries[i] = (page_table_t*)(table_addr | 3);
	}
	kernel_dir->entries[last_page] = (page_table_t*)(((uintptr_t)&kernel_dir->tables_phy[last_page]) | 3);
}
__cdecl void page_init_dir(page_dir_t *dir)
{
	dir->phy_addr = (uint32_t)&(dir->entries);
	memset((void*)dir->tables_phy, 0, sizeof(dir->tables_phy));
	for (int i = 0; i < 1024; i++)
	{
		uintptr_t table_addr = (uintptr_t)(&dir->tables_phy[i]);
		dir->entries[i] = (page_table_t*)(table_addr | 3);
	}
	for (int i = 0x0; i < ((uint32_t)&kend); i += 0x1000)
		page_map_addr_dir(i, i, dir, {present: true, rw: read_write, privilege: supervisor, 0, accessed: true, dirty: true, 0, 0});
	dprintf("initialized directory.\n");
}

__cdecl void page_map_addr(uint32_t phy, uint32_t virt, page_table_entry_t table_entry)
{
	uint32_t id = virt >> 22;
	uint32_t table_id = (virt >> 12) & 0x3FF;
		
	for (int i = 0; i < (1024-table_id); i++)
	{
		current_dir->tables_phy[id].entries[table_id+i].block_addr = (phy >> 12) + (i * 0x1000);

		current_dir->tables_phy[id].entries[table_id+i].present = table_entry.present;
		current_dir->tables_phy[id].entries[table_id+i].rw = table_entry.rw;
		current_dir->tables_phy[id].entries[table_id+i].privilege = table_entry.privilege;
		current_dir->tables_phy[id].entries[table_id+i].accessed = table_entry.accessed;
		current_dir->tables_phy[id].entries[table_id+i].dirty = table_entry.dirty;
		current_dir->tables_phy[id].entries[table_id+i].free = false;

		current_dir->tables_phy[id].entries[table_id+i].reserved_1 = 0;
		current_dir->tables_phy[id].entries[table_id+i].reserved_2 = 0;
		current_dir->tables_phy[id].entries[table_id+i].unused = 0;
	}

	page_table_entry_t *e = (page_table_entry_t *)&current_dir->entries[id];
	e->present = table_entry.present;
	e->rw = table_entry.rw;
	e->privilege = table_entry.privilege;
}
__cdecl void page_map_addr_sz(uint32_t phy, uint32_t virt, size_t sz, page_table_entry_t table_entry)
{
	for (uintptr_t i = 0; i < sz; i += 0x1000)
	{
		page_map_addr(phy + i, virt + i, table_entry);
	}
}

__cdecl void page_map_addr_dir(uint32_t phy, uint32_t virt, page_dir_t *dir, page_table_entry_t table_entry)
{
	uint32_t id = virt >> 22;
	uint32_t table_id = (virt >> 12) & 0x3FF;
		
	for (int i = 0; i < (1024-table_id); i++)
	{
		dir->tables_phy[id].entries[table_id+i].block_addr = (phy >> 12) + (i * 0x1000);

		dir->tables_phy[id].entries[table_id+i].present = table_entry.present;
		dir->tables_phy[id].entries[table_id+i].rw = table_entry.rw;
		dir->tables_phy[id].entries[table_id+i].privilege = table_entry.privilege;
		dir->tables_phy[id].entries[table_id+i].accessed = table_entry.accessed;
		dir->tables_phy[id].entries[table_id+i].dirty = table_entry.dirty;
		dir->tables_phy[id].entries[table_id+i].free = false;

		dir->tables_phy[id].entries[table_id+i].reserved_1 = 0;
		dir->tables_phy[id].entries[table_id+i].reserved_2 = 0;
		dir->tables_phy[id].entries[table_id+i].unused = 0;
	}

	page_table_entry_t *e = (page_table_entry_t *)&dir->entries[id];
	e->present = table_entry.present;
	e->rw = table_entry.rw;
	e->privilege = table_entry.privilege;
}
__cdecl void page_map_addr_dir_sz(uint32_t phy, uint32_t virt, page_dir_t *dir, size_t sz, page_table_entry_t table_entry)
{
	for (uintptr_t i = 0; i < sz; i += 0x1000)
	{
		page_map_addr_dir(phy + i, virt + i, dir, table_entry);
	}
}

__cdecl void page_unmap_addr(uint32_t virt)
{
	uint32_t id = virt >> 22;
	uint32_t table_id = (virt >> 12) & 0x3FF;
		
	for (int i = 0; i < (1024-table_id); i++)
	{
		current_dir->tables_phy[id].entries[table_id+i].block_addr = 0;

		current_dir->tables_phy[id].entries[table_id+i].present = false;
		current_dir->tables_phy[id].entries[table_id+i].rw = read_only;
		current_dir->tables_phy[id].entries[table_id+i].privilege = supervisor;
		current_dir->tables_phy[id].entries[table_id+i].accessed = false;
		current_dir->tables_phy[id].entries[table_id+i].dirty = false;
		current_dir->tables_phy[id].entries[table_id+i].free = true;

		current_dir->tables_phy[id].entries[table_id+i].reserved_1 = 0;
		current_dir->tables_phy[id].entries[table_id+i].reserved_2 = 0;
		current_dir->tables_phy[id].entries[table_id+i].unused = 0;
	}
}
__cdecl void page_unmap_addr_sz(uint32_t virt, size_t sz)
{
	for (uintptr_t i = 0; i < sz; i += 0x1000)
	{
		page_unmap_addr(virt + i);
	}
}

__cdecl void page_unmap_addr_dir(uint32_t virt, page_dir_t *dir)
{
	uint32_t id = virt >> 22;
	uint32_t table_id = (virt >> 12) & 0x3FF;
		
	for (int i = 0; i < (1024-table_id); i++)
	{
		dir->tables_phy[id].entries[table_id+i].block_addr = 0;

		dir->tables_phy[id].entries[table_id+i].present = false;
		dir->tables_phy[id].entries[table_id+i].rw = read_only;
		dir->tables_phy[id].entries[table_id+i].privilege = supervisor;
		dir->tables_phy[id].entries[table_id+i].accessed = false;
		dir->tables_phy[id].entries[table_id+i].dirty = false;
		dir->tables_phy[id].entries[table_id+i].free = true;

		dir->tables_phy[id].entries[table_id+i].reserved_1 = 0;
		dir->tables_phy[id].entries[table_id+i].reserved_2 = 0;
		dir->tables_phy[id].entries[table_id+i].unused = 0;
	}
}
__cdecl void page_unmap_addr_dir_sz(uint32_t virt, page_dir_t *dir, size_t sz)
{
	for (uintptr_t i = 0; i < sz; i += 0x1000)
	{
		page_unmap_addr_dir(virt + i, dir);
	}
}

__cdecl page_table_t *page_table_alloc(size_t tables_count)
{
	loop:
		if (last_page + tables_count > 1024)
		{
			return nullptr;
		}
		bool free = true;
		int ind = 0;
		for (int i = last_page + tables_count, s = 0; i < 1024; i--,s++)
		{
			if (!current_dir->entries[i]->entries[s].free)
			{
				free = false;
				ind = i;
				break;
			}
		}
	if (!free)
	{
		last_page = ind;
		goto loop;
	}
	last_page += tables_count;
	return &current_dir->tables_phy[ind];
}
__cdecl void *page_alloc(size_t pages_count)
{
	page_table_t *table = page_table_alloc(pages_count);
	if (table == nullptr)
	{
		return nullptr;
	}
	uintptr_t addr = (uintptr_t)table->entries[0].block_addr;
	return (void*)addr;
}

__cdecl void paging_enable()
{
	for (int i = 0x0; i < ((uint32_t)&kend); i += 0x1000)
		page_map_addr(i, i, {present: true, rw: read_write, privilege: supervisor, 0, accessed: true, dirty: true, 0, 0});

	//print("paging_enable() >> Mapped kernel\n");

	asm volatile("mov $0, %ebx");
	__page_dir_flush();
	__page_set_cr0();
	
	//print("paging_enable() >> Paging enabled\n");
}
__cdecl void paging_disable()
{
	int cr0;
	asm("mov %%cr0, %0" : "=r"(cr0));
	cr0 &= ~0x80000000;
	asm("mov %0, %%cr0" : : "r"(cr0));
	//print("paging_disable() >> Paging disabled\n");
	asm("sti");
}