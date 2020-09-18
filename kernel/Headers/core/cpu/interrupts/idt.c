#include "Headers/core/cpu/interrupts/idt.h"
#include "Headers/utility/util.h"

uint16_t low_16(uint32_t content){
	return (content & 0xFFFF);
}

uint16_t high_16(uint32_t content){
	return (content >> 16);
}

void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = low_16(handler);
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; 
    idt[n].high_offset = high_16(handler);
}

void set_idt() {
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_reg));
}
