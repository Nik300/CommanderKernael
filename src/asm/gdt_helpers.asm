[BITS 32]
[SECTION .text]
[global __reload_regs]
[global __gdt_flush]
[global __idt_flush]
[global __page_dir_flush]
[global __page_set_cr0]
[extern __gdtr]
[extern _idtr]
[extern dir_loc]
[extern kstack_top]

__gdt_flush:
	mov		eax, [__gdtr]
	lgdt    [eax]
	jmp 0x08:__gdt_flush_end
__gdt_flush_end:
	mov ax, (5 * 8) | 0
	ltr ax
	ret

__idt_flush:
	mov		eax, [_idtr]
	lidt    [eax]
	;jmp $
	jmp 0x08:__idt_flush_end
__idt_flush_end:
	ret

__page_dir_flush:
	mov		eax, [dir_loc]
	mov		cr3, eax

_continue:
	jmp 0x08:__page_dir_flush_end
__page_dir_flush_end:
	ret

__page_set_cr0:
	mov		eax, cr0
	or		eax, 0x80000000
	mov		cr0, eax
	jmp 0x08:__page_set_cr0_end
__page_set_cr0_end:
	ret

__reload_regs:
	mov ax, 0x10
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ss, ax
	jmp 0x08:__reload_segments_end
__reload_segments_end:
	ret