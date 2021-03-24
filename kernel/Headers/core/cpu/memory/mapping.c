//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#include "Headers/utility/types.h"
#include "Headers/utility/ports.h"

uint32_t count_memory()
{
	register uint64_t *mem;
	uint64_t	mem_count, a;
	uint32_t	memkb;
	uint8_t		irq1, irq2;
	uint64_t	cr0;
 
	/* save IRQ's */
	irq1=port_byte_in(0x21);
	irq2=port_byte_in(0xA1);
 
	/* kill all irq's */
	port_byte_out(0x21, 0xFF);
	port_byte_out(0xA1, 0xFF);
 
	mem_count=0;
	memkb=0;
 
	// store a copy of CR0
	//__asm__ __volatile("movl %%cr0, %%eax":"=a"(cr0))::"eax");
 
	// invalidate the cache
	// write-back and invalidate the cache
	__asm__ __volatile__ ("wbinvd");
 
	// plug cr0 with just PE/CD/NW
	// cache disable(486+), no-writeback(486+), 32bit mode(386+)
	//__asm__ __volatile__("movl %%eax, %%cr0", ::
	//	"a" (cr0 | 0x00000001 | 0x40000000 | 0x20000000) : "eax");
 
	do {
		memkb++;
		mem_count += 1024*1024;
		mem= (uint64_t*) mem_count;
 
		a= *mem;
		*mem= 0x55AA55AA;
 
          // the empty asm calls tell gcc not to rely on what's in its registers
          // as saved variables (this avoids GCC optimisations)
		asm("":::"memory");
		if (*mem!=0x55AA55AA) mem_count=0;
		else {
			*mem=0xAA55AA55;
			asm("":::"memory");
			if(*mem!=0xAA55AA55)
			mem_count=0;
		}
 
		asm("":::"memory");
		*mem=a;
 
	} while (memkb<4096 && mem_count!=0);
 
	//__asm__ __volatile__("movl %%eax, %%cr0", :: "a" (cr0) : "eax");
 
	//mem_end = memkb<<20;
	mem = (uint64_t*) 0x413;
	//bse_end= (*mem & 0xFFFF) <<6;
 
	port_byte_out(0x21, irq1);
	port_byte_out(0xA1, irq2);

	return memkb;
}