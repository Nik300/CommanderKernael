#include <Headers/utility/types.h>
#include <Headers/utility/paging.h>
#include <Headers/utility/kalloc.h>
#include <Headers/utility/frame.h>
#include <Headers/utility/util.h>

page_directory* kernel_directory;
page_directory* current_directory;
extern uint32_t placement_address;

bool initialized = false;

void identity_map(uint32_t address, uint32_t lenght) {
	uint32_t curr = address & 0xFFFFF000;
	while (curr < address + lenght) {
		page* p = get_page(curr, 1, kernel_directory);
		p->present = 1;
		p->rw = 1;
		p->user = 0;
		p->frame = curr >> 12;

		curr += 0x1000;
	}
}

void initialize_paging(uint32_t total_frames, uint32_t ident_addr, uint32_t ident_len) {
	init_frame_allocator(total_frames);
	
	kernel_directory = (page_directory*)e_kmalloc_a(sizeof(page_directory));
	uint32_t i;
	memset((char*)kernel_directory, 0, sizeof(page_directory));
	current_directory = kernel_directory;

	for (i = 0; i < 0xFFFFFFFF;) {
		get_page(i, 1, kernel_directory);
		i += 0x1000 * 1024;
		if (i == 0) break;
	}

	i = 0;

	while (i < placement_address) {
		alloc_frame(get_page(i, 1, kernel_directory), false, false);
		i += 0x1000;
	}

	uint32_t heap_start = disable_early_kmalloc();

	for (i = 0; i < 4; i++) {
		alloc_frame(get_page(heap_start + (i * 0x1000), 1, kernel_directory), false, false);
	}

	register_interrupt_handler(14, page_fault);

	if (ident_addr) {
		identity_map(ident_addr, ident_len);
	}


	switch_page_directory(kernel_directory);
	initialized = 1;

}

void switch_page_directory(page_directory* dir) {
	current_directory = dir;
	asm volatile("mov %0, %%cr3":: "r"((uint32_t)&dir->tablesPhysical));
	uint32_t cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; // Enable paging!
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page* get_page(uint32_t address, int make, page_directory* dir) {
	address /= 0x1000;

	uint32_t table_idx = address / 1024;
	if (dir->tables[table_idx]) return &dir->tables[table_idx]->pages[address % 1024];
	else if (make) {
		uint32_t tmp;
		if (!initialized)
			dir->tables[table_idx] = (page_table*)e_kmalloc_ap(sizeof(page_table), &tmp);
		else
			return;

		memset((char*)dir->tables[table_idx], 0, 0x1000);
		dir->tablesPhysical[table_idx] = tmp | 0x7;
		return &dir->tables[table_idx]->pages[address % 1024];
	}
	else {
		return NULL;
	}
}

page* get_kernel_page(uint32_t address, int make) {
	return get_page(address, make, kernel_directory);
}

page* map_kernel_page(uint32_t address, int make) {
	page* p = get_page(address, make, kernel_directory);
	if (!p) return NULL;
	alloc_frame(p, false, false);
	return p;
}

void unmap_kernel_page(uint32_t address) {
	page* p = get_page(address, 0, kernel_directory);
	if (p) {
		free_frame(p);
	}
}