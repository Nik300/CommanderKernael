[BITS 32]
[GLOBAL _start]
[EXTERN main]

[SECTION .text.start]
_start:
	call main   ; call main function
	mov ebx, eax; set exit code
	mov eax, 0  ; set syscall 0 (exit)
	int 0x30	; syscall
	jmp $		; infinite loop, just in case