#ifndef __PAGING_H_
#define __PAGING_H_

#include <Headers/utility/types.h>
#include <Headers/core/cpu/interrupts/isr.h>

typedef struct {
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t user : 1;
	uint32_t accessed : 1;
	uint32_t dirty : 1;
	uint32_t unused : 7;
	uint32_t frame : 21;
} page;

typedef struct {
	page pages[1024];
} page_table;

typedef struct {
	page_table* tables[1024];

	uint32_t tablesPhysical[1024];

	uint32_t physicalAddr;
} page_directory;

void initialize_paging(uint32_t total_frames, uint32_t ident_addr, uint32_t ident_len);
void switch_page_directory(page_directory* npage);
page* get_page(uint32_t address, int make, page_directory *dir);
page* get_kernel_page(uint32_t address, int make);
page* map_kernel_page(uint32_t address, int make);
void unmap_kernel_page(uint32_t address);

void page_fault(registers_t r);

#endif // __PAGING_H_
