#include <lib/gdt.h>
#include <lib/idt.hpp>
#include <lib/memory.h>

extern "C" gdt32_entry_t *gdt32_table = nullptr;
extern "C" gdt_register_t *__gdtr = nullptr;
extern "C" tss_entry_struct_t __tss = {};

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}