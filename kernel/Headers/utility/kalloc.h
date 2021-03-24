#ifndef __KALLOC_H_
#define __KALLOC_H_

#include <Headers/utility/types.h>

uint32_t e_kmalloc_a(uint32_t sz);                  // page aligned.
uint32_t e_kmalloc_p(uint32_t sz, uint32_t* phys);  // returns a physical address.
uint32_t e_kmalloc_ap(uint32_t sz, uint32_t* phys); // page aligned and returns a physical address.
uint32_t e_kmalloc(uint32_t sz);                    // vanilla (normal).

uint32_t disable_early_kmalloc();

#endif //__KALLOC_H_