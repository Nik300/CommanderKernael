[SECTION .text]
[extern fault_handler]
[extern interrupt_handler]

%macro isr 1
	[global isr_%1]
	isr_%1:
		; disable paging
		push dword %1
		
		push eax
		push ecx
		push edx
		push ebx

		push esp
		push ebp
		push esi
		push edi

		mov dword eax, 0
		mov ax, ds
		push eax

		mov ax, 0x10
			mov ds, ax
			mov es, ax
			mov fs, ax
			mov gs, ax
		
		call fault_handler

		pop eax
			mov ds, ax
			mov es, ax
			mov fs, ax
			mov gs, ax

		pop edi
		pop esi
		pop ebp
		pop esp

		pop ebx	
		pop edx
		pop ecx
		pop eax

		add esp, 4

		sti
		iret
%endmacro
%macro irq 1
	[global irq_%1]
	irq_%1:
		push dword %1
		jmp 0x08:common_irq_stub
%endmacro

isr 0
isr 1
isr 2
isr 3
isr 4
isr 5
isr 6
isr 7
isr 8
isr 9
isr 10
isr 11
isr 12
isr 13
isr 14
isr 15
isr 16
isr 17
isr 18
isr 19
isr 20
isr 21
isr 22
isr 23
isr 24
isr 25
isr 26
isr 27
isr 28
isr 29
isr 30
isr 31

irq 0  ; pit
irq 1  ; keyboard
irq 2  ; cascade
irq 3  ; com2
irq 4  ; com1
irq 5  ; lpt2
irq 6  ; floppy
irq 7  ; lpt1
irq 8  ; rtc
irq 9  ; mouse
irq 10 ; fpu
irq 11 ; ata1
irq 12 ; ata2
irq 13 ; ps2
irq 14 ; cascade
irq 15 ; com3
irq 16 ; syscall

n_called: db dword 0

common_irq_stub:
	cli
	push eax
	push ecx
	push edx
	push ebx

	push esp
	push ebp
	push esi
	push edi

	mov dword eax, 0
	mov ax, ds
	push eax

	mov ax, 0x10
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax

	call interrupt_handler
	
	pop eax
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax

	pop edi
	pop esi
	pop ebp
	pop esp

	pop ebx
	pop edx
	pop ecx
	pop eax

	add esp, 4

	sti
	iret