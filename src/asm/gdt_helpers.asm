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
[global test_umode]
test2:
	mov dword eax, 1
	push eax
	pop ebx
	ret
user_esp:
	resb 4*1024
test_user_function:
	xchg bx, bx
	push dword 1
	pop eax
	;jmp $
	int 1
	mov dword eax, 4
	push eax
	pop ebx
	jmp $
	int 	1
	call test2
	jmp $
test_umode:
	cli
	mov ax, (4 * 8) | 3 ; ring 3 data with bottom 2 bits set for ring 3
		mov ds, ax
		mov es, ax 
		mov fs, ax 
		mov gs, ax
		
	; set up the stack frame iret expects
	mov eax, user_esp
	push (4 * 8) | 3 ; data selector
	push eax ; current esp
	pushf ; eflags
	pop eax
		or eax, 0x200
	push eax
	push (3 * 8) | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	push test_user_function ; instruction address to return to
	xchg bx, bx
	iret