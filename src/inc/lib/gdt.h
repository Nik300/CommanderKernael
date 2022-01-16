#pragma once
#include <stdint.h>
#include <stddef.h>
#include <lib/memory.h>

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#define KERNEL_CODE_INDEX 1
#define KERNEL_DATA_INDEX 2

#define USER_CODE_INDEX   3
#define USER_DATA_INDEX   4

#define TSS_INDEX 5

#define MODULE_CODE_INDEX 6
#define MODULE_DATA_INDEX 7

#define DRIVER_CODE_INDEX 8
#define DRIVER_DATA_INDEX 9

#define GDT_ENTRIES 10

typedef enum {
	GDT32_DIR_UPWARDS = 0,
	GDT32_DIR_DOWNWARDS = 1
} gdt32_direction_bit;
typedef enum {
	GDT32_CONF 	  = 0,
	GDT32_NONCONF = 1
} gdt32_conforming_bit;
typedef enum {
	GDT32_ALIGN_1B = 0,
	GDT32_ALIGN_4K = 1
} gdt32_granularity_bit;
typedef enum {
	GDT32_16BIT = 0,
	GDT32_32BIT = 1
} gdt32_size_bit;

typedef struct gdt32_entry_access
{
	uint8_t accessed : 1;
	uint8_t read_write : 1;
	uint8_t conforming_direction : 1;
	uint8_t executable : 1;
	uint8_t type : 1;
	uint8_t privilege_level : 2;
	uint8_t present : 1;
} __attribute__((packed)) gdt32_entry_access_t;
typedef struct gdt32_entry_flags {
	uint8_t reserved : 1;
	bool long_mode : 1;
	gdt32_size_bit size : 1;
	gdt32_granularity_bit granularity : 1;		
} __attribute__((packed)) gdt32_entry_flags_t;

typedef struct gdt32_entry
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	
	uint8_t accessed : 1;
	uint8_t read_write : 1;
	uint8_t conforming_direction : 1;
	uint8_t executable : 1;
	uint8_t type : 1;
	uint8_t privilege_level : 2;
	uint8_t present : 1;
	
	uint8_t limit_high : 4; // aka limit_high/flags

	uint8_t reserved : 1;
	bool long_mode : 1;
	gdt32_size_bit size : 1;
	gdt32_granularity_bit granularity : 1;		

	uint8_t base_high;
} __attribute__((packed)) gdt32_entry_t;
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_register_t;
typedef struct tss_entry_struct {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed)) tss_entry_struct_t;
extern gdt_register_t *__gdtr;
extern gdt32_entry_t *gdt32_table;
extern tss_entry_struct_t *tss_sect;
extern tss_entry_struct_t __tss;
extern void *kstack_top;

static inline void set_ldr()
{
	
}

static inline void set_descriptor(int n, uint32_t base, uint32_t limit, gdt32_entry_access_t access, gdt32_entry_flags_t flags)
{
	gdt32_table[n].base_low = base & 0xFFFF;
	gdt32_table[n].base_middle = (base >> 16) & 0xFF;
	gdt32_table[n].base_high = (base >> 24) & 0xFF;
	
	gdt32_table[n].limit_low = limit & 0xFFFF;
	gdt32_table[n].limit_high = (limit >> 16) & 0x0F;

	gdt32_table[n].granularity = flags.granularity;
	gdt32_table[n].size = flags.size;
	gdt32_table[n].long_mode = flags.long_mode;
	gdt32_table[n].reserved = 0;

	gdt32_table[n].accessed = access.accessed;
	gdt32_table[n].read_write = access.read_write;
	gdt32_table[n].conforming_direction = access.conforming_direction;
	gdt32_table[n].executable = access.executable;
	gdt32_table[n].type = access.type;
	gdt32_table[n].privilege_level = access.privilege_level;
	gdt32_table[n].present = access.present;
}

static inline void init_entries()
{
	// null descriptor
	set_descriptor(0, 0, 0, (gdt32_entry_access_t){
		.accessed = 0,
		.read_write = 0,
		.conforming_direction = 0,
		.executable = 0,
		.type = 0,
		.privilege_level = 0,
		.present = 0
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_16BIT,
		.granularity = GDT32_ALIGN_1B
	});

	// kernel code segment
	set_descriptor(KERNEL_CODE_INDEX, 0, 0xFFFFFFFF, (gdt32_entry_access_t)
	{
		.accessed = 0,
		.read_write = 1,
		.conforming_direction = 0,
		.executable = 1,
		.type = 1,
		.privilege_level = 0,
		.present = 1
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_32BIT,
		.granularity = GDT32_ALIGN_4K
	});
	// kernel data segment
	set_descriptor(KERNEL_DATA_INDEX, 0, 0xFFFFFFFF, (gdt32_entry_access_t) 
	{
		.accessed = 0,
		.read_write = 1,
		.conforming_direction = 0,
		.executable = 0,
		.type = 1,
		.privilege_level = 0,
		.present = 1
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_32BIT,
		.granularity = GDT32_ALIGN_4K
	});

	// module code segment
	set_descriptor(MODULE_CODE_INDEX, 0, 0xFFFFFFFF, (gdt32_entry_access_t)
	{
		.accessed = 0,
		.read_write = 1,
		.conforming_direction = 0,
		.executable = 1,
		.type = 1,
		.privilege_level = 1,
		.present = 1
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_32BIT,
		.granularity = GDT32_ALIGN_4K
	});
	// module data segment
	set_descriptor(MODULE_DATA_INDEX, 0, 0xFFFFFFFF, (gdt32_entry_access_t)
	{
		.accessed = 0,
		.read_write = 1,
		.conforming_direction = 0,
		.executable = 0,
		.type = 1,
		.privilege_level = 1,
		.present = 1
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_32BIT,
		.granularity = GDT32_ALIGN_4K
	});

	// driver code segment
	set_descriptor(DRIVER_CODE_INDEX, 0, 0xFFFFFFFF, (gdt32_entry_access_t)
	{
		.accessed = 0,
		.read_write = 1,
		.conforming_direction = 0,
		.executable = 1,
		.type = 1,
		.privilege_level = 2,
		.present = 1
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_32BIT,
		.granularity = GDT32_ALIGN_4K
	});
	// driver data segment
	set_descriptor(DRIVER_DATA_INDEX, 0, 0xFFFFFFFF, (gdt32_entry_access_t)
	{
		.accessed = 0,
		.read_write = 1,
		.conforming_direction = 0,
		.executable = 0,
		.type = 1,
		.privilege_level = 2,
		.present = 1
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_32BIT,
		.granularity = GDT32_ALIGN_4K
	});

	// kernel code segment
	set_descriptor(USER_CODE_INDEX, 0, 0xFFFFFFFF, (gdt32_entry_access_t)
	{
		.accessed = 0,
		.read_write = 1,
		.conforming_direction = 1,
		.executable = 1,
		.type = 1,
		.privilege_level = 3,
		.present = 1
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_32BIT,
		.granularity = GDT32_ALIGN_4K
	});
	// kernel data segment
	set_descriptor(USER_DATA_INDEX, 0, 0xFFFFFFFF, (gdt32_entry_access_t) 
	{
		.accessed = 0,
		.read_write = 1,
		.conforming_direction = 0,
		.executable = 0,
		.type = 1,
		.privilege_level = 3,
		.present = 1
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_32BIT,
		.granularity = GDT32_ALIGN_4K
	});

	// set TSS
	set_descriptor(TSS_INDEX, (uint32_t)&__tss, sizeof(tss_entry_struct_t), (gdt32_entry_access_t)
	{
		.accessed = 1,
		.read_write = 0,
		.conforming_direction = 0,
		.executable = 1,
		.type = 0,
		.privilege_level = 0,
		.present = 1
	}, (gdt32_entry_flags_t){
		.reserved = 0,
		.long_mode = 0,
		.size = GDT32_16BIT,
		.granularity = GDT32_ALIGN_1B
	});
		memset(&__tss, 0, sizeof(tss_entry_struct_t));

	__tss.ss0  = (KERNEL_DATA_INDEX * 8) | 0;
	__tss.ss1  = (MODULE_DATA_INDEX * 8) | 1;
	__tss.ss2  = (DRIVER_DATA_INDEX * 8) | 2;
	int esp;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	__tss.esp0 = (uint32_t) esp;
	__tss.cs = 0x8;
	__tss.ds = 0x10;
	__tss.es = 0x10;
	__tss.fs = 0x10;
	__tss.gs = 0x10;
	__tss.ss = 0x10;
}

extern void *gdt_table;

extern void __reload_regs();
extern void __gdt_flush();
static inline void load_gdt()
{
	asm("xchg %bx, %bx");
	gdt_register_t gdtr = {};
	__gdtr = (gdt_register_t*)&gdtr;
	gdt32_table = (gdt32_entry_t*)(&gdt_table);

	init_entries();

	__gdtr->base = (uint32_t) gdt32_table;
    __gdtr->limit = (sizeof(gdt32_entry_t) * GDT_ENTRIES);
	
	__gdt_flush();
	__reload_regs();
}

static inline uint16_t get_kernel_code_segment()
{
	return KERNEL_CODE_INDEX * 8;
}
static inline uint16_t get_kernel_data_segment()
{
	return KERNEL_DATA_INDEX * 8;
}
static inline uint16_t get_module_code_segment()
{
	return (MODULE_CODE_INDEX * 8) | 1;
}
static inline uint16_t get_module_data_segment()
{
	return (MODULE_DATA_INDEX * 8) | 1;
}
static inline uint16_t get_driver_code_segment()
{
	return (DRIVER_CODE_INDEX * 8) | 2;
}
static inline uint16_t get_driver_data_segment()
{
	return (DRIVER_DATA_INDEX * 8) | 2;
}
static inline uint16_t get_user_code_segment()
{
	return (USER_CODE_INDEX * 8) | 3;
}
static inline uint16_t get_user_data_segment()
{
	return (USER_DATA_INDEX * 8) | 3;
}

#ifdef __cplusplus
}
#endif