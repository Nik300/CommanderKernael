#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <std/stddecl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { read_only, read_write } access_t;
typedef enum { supervisor, user } privilege_t;
typedef enum { pgsz_4KB, pgsz_4MB } page_size_t;

typedef struct __packed page_table_entry
{
	bool 		present	  : 1; // Indicates whether the page is present in memory
	access_t 	rw		  : 1; // Indicates whether the page is read-only or read-write
	privilege_t privilege : 1; // Indicates whether the page is accessible to user or supervisor
	uint8_t     reserved_1: 2; // Reserved bits
	bool 		accessed  : 1; // Indicates whether the page has been accessed
	bool 		dirty	  : 1; // Indicates whether the page has been written to
	uint32_t 	reserved_2: 2; // Reserved for internal use, cannot be modified
	uint32_t 	unused    : 3; // Unused bits, available for kernel use
	uintptr_t	block_addr: 20;// The address of the page
} __packed page_table_entry_t;
typedef struct __packed page_dir_entry
{
	bool        present   : 1; // Indicates whether the table is present in memory
	access_t    rw        : 1; // Indicates whether the table is read-only or read-write
	privilege_t privilege : 1; // Indicates whether the table is accessible to user or supervisor
	uint8_t     reserved_1: 2; // Reserved bits
	bool        accessed  : 1; // Indicates whether the table has been accessed
	bool        dirty     : 1; // Indicates whether the table has been written to
	page_size_t page_size : 1; // Indicates whether the table is 4KB or 4MB
	uint8_t		unused    : 4; // Unused bits, available for kernel use
	uintptr_t   table_addr: 20; // The address of the table

} __packed page_dir_entry_t;

typedef struct __packed page_table
{
	page_table_entry_t entries[1024];
} __packed page_table_t;
typedef struct __packed page_dir
{
	page_dir_entry_t entries[1024];
} __packed page_dir_t;

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
void page_map_addr_one_pg(uint32_t phy, uint32_t virt, page_table_entry_t table_entry = {
	.present = true,
	.rw = read_write,
	.privilege = supervisor,
	.reserved_1 = 0,
	.accessed = false,
	.dirty = false
});
void page_map_addr_one_pg_sz(uint32_t phy, uint32_t virt, size_t sz, page_table_entry_t table_entry = {
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
void page_map_addr_dir_one_pg(uint32_t phy, uint32_t virt, page_dir_t *dir, page_table_entry_t table_entry = {
	.present = true,
	.rw = read_write,
	.privilege = supervisor,
	.reserved_1 = 0,
	.accessed = false,
	.dirty = false
});
void page_map_addr_dir_one_pg_sz(uint32_t phy, uint32_t virt, page_dir_t *dir, size_t sz, page_table_entry_t table_entry = {
	.present = true,
	.rw = read_write,
	.privilege = supervisor,
	.reserved_1 = 0,
	.accessed = false,
	.dirty = false
});
#else
void page_map_addr(uint32_t phy, uint32_t virt, page_table_entry_t table_entry);
void page_map_addr_sz(uint32_t phy, uint32_t virt, size_t sz, page_table_entry_t table_entry);
void page_map_addr_one_pg(uint32_t phy, uint32_t virt, page_table_entry_t table_entry);
void page_map_addr_one_pg_sz(uint32_t phy, uint32_t virt, size_t sz, page_table_entry_t table_entry);

void page_map_addr_dir(uint32_t phy, uint32_t virt, page_dir_t *dir, page_table_entry_t table_entry);
void page_map_addr_dir_sz(uint32_t phy, uint32_t virt, page_dir_t *dir, size_t sz, page_table_entry_t table_entry);
void page_map_addr_dir_one_pg(uint32_t phy, uint32_t virt, page_dir_t *dir, page_table_entry_t table_entry);
void page_map_addr_dir_one_pg_sz(uint32_t phy, uint32_t virt, page_dir_t *dir, size_t sz, page_table_entry_t table_entry);
#endif
void page_unmap_addr(uint32_t virt);
void page_unmap_addr_sz(uint32_t virt, size_t sz);
void page_unmap_addr_one_pg(uint32_t virt);
void page_unmap_addr_one_pg_sz(uint32_t virt, size_t sz);
void page_unmap_addr_dir(uint32_t virt, page_dir_t *dir);
void page_unmap_addr_dir_sz(uint32_t virt, page_dir_t *dir, size_t sz);
void page_unmap_addr_dir_one_pg(uint32_t virt, page_dir_t *dir);
void page_unmap_addr_dir_one_pg_sz(uint32_t virt, page_dir_t *dir, size_t sz);
uintptr_t mem_align(uintptr_t addr);
uintptr_t mem_dealign(uintptr_t addr);
void paging_disable();
page_dir_t *get_kernel_dir();
page_dir_t *get_current_dir();
void page_switch_dir(page_dir_t *dir);
void page_init_kdir();
page_dir_t *page_create_dir();
void page_init_dir (page_dir_t *dir);
void page_destroy_dir(page_dir_t *dir);
void paging_init();
void paging_enable();
void paging_disable();
#ifdef __cplusplus
}
#endif