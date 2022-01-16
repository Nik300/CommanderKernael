#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { read_only, read_write } access_t;
typedef enum { supervisor, user } privilege_t;

typedef struct page_table_entry
{
	bool 		present	  : 1; // Indicates whether the page is present in memory
	access_t 	rw		  : 1; // Indicates whether the page is read-only or read-write
	privilege_t privilege : 1; // Indicates whether the page is accessible to user or supervisor
	uint8_t     reserved_1: 2; // Reserved bits
	bool 		accessed  : 1; // Indicates whether the page has been accessed
	bool 		dirty	  : 1; // Indicates whether the page has been written to
	uint32_t 	reserved_2: 2; // Reserved for internal use, cannot be modified
	uint8_t		free      : 1; // Indicates if the page is free or not
    uint32_t 	unused    : 2; // Unused bits, available for kernel use
	uintptr_t	block_addr: 20;// The address of the page
} __attribute__((packed)) page_table_entry_t;
typedef struct page_table
{
	page_table_entry_t entries[1024];
} __attribute__((packed)) page_table_t;
typedef struct page_dir
{
	page_table_t *entries[1024];
	page_table_t tables_phy[1024];
	uint32_t	 phy_addr;
} __attribute__((packed)) page_dir_t;

void page_dir_init();
#ifdef __cplusplus
void page_map_addr(uint32_t phy, uint32_t virt, page_table_entry_t table_entry = {
	.present = true,
	.rw = read_write,
	.privilege = supervisor,
	.reserved_1 = 0,
	.accessed = false,
	.dirty = false
});
void page_map_addr_sz(uint32_t phy, uint32_t virt, size_t sz, page_table_entry_t table_entry = {
	.present = true,
	.rw = read_write,
	.privilege = supervisor,
	.reserved_1 = 0,
	.accessed = false,
	.dirty = false
});
void page_map_addr_dir(uint32_t phy, uint32_t virt, page_dir_t *dir, page_table_entry_t table_entry = {
	.present = true,
	.rw = read_write,
	.privilege = supervisor,
	.reserved_1 = 0,
	.accessed = false,
	.dirty = false
});
void page_map_addr_dir_sz(uint32_t phy, uint32_t virt, page_dir_t *dir, size_t sz, page_table_entry_t table_entry = {
	.present = true,
	.rw = read_write,
	.privilege = supervisor,
	.reserved_1 = 0,
	.accessed = false,
	.dirty = false
});
uintptr_t mem_align(void *addr);
uintptr_t mem_dealign(void *addr);
void page_unmap_addr(uint32_t virt);
void page_unmap_addr_sz(uint32_t virt, size_t sz);
void page_unmap_addr_dir(uint32_t virt, page_dir_t *dir);
void page_unmap_addr_dir_sz(uint32_t virt, page_dir_t *dir, size_t sz);
void paging_disable();
page_dir_t *get_kernel_dir();
void page_switch_dir(page_dir_t *dir);
void page_init_dir(page_dir_t *dir);
#else
void page_map_addr(uint32_t phy, uint32_t virt, page_table_entry_t table_entry);
#endif
void paging_enable();
void paging_disable();
page_table_t *page_table_alloc(size_t tables_count);
void *page_alloc(size_t pages_count);
#ifdef __cplusplus
}
#endif