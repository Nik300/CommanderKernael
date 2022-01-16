/*CONSTS*/
.set ALIGN, 1<<0                // align kernel modules on page boundaries
.set MEMORY_INFO, 1<<1           // tells grub to map memory
.set FLAGS, ALIGN|MEMORY_INFO   // combine flags
.set MAGIC, 0x1BADB002          // grub's magic number
.set CHECKSUM, -(MAGIC + FLAGS) // checksum

.set KSTACK_SZ, 4*(1024*1024)   // kernel stack size 4MB
.set KHEAP_SZ,  100*(1024*1024) // kernel heap size 100MB
.set PHEAP_SZ,  4202500			// page heap size (sizeof(page_dir_t))
.set IDTTB_SZ,	2048			// idttb size (sizeof(idttb_t))
.set GDTTB_SZ,	2048			// gdttb size (sizeof(gdttb_t))
.set TSS_SZ,	104				// tss size (sizeof(tss_t))

/*
    Now that we have all of the needed constants, we need
    to define them into memory, so that multiboot can actually
    see and recognize them.
*/
.section .multiboot
    .global kstart
    kstart:
    .align 4 // align to 4 bytes
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

/*
    OK, now we can define bss and data sections.
*/
.section .bss
    // define kernel stack
    .align 4 // align to 16 bytes - so to make sure it doesn't overlap with the multiboot info
    .global kstack_top
    .global kheap
	.global kheap_e
	.global pheap
	.global pheap_e

    kstack_bottom:
        .space KSTACK_SZ
    kstack_top:
    kheap:
		.space KHEAP_SZ
	kheap_e:
	pheap:
		.space PHEAP_SZ
	pheap_e:
	
.section .data
    .global kstack_size
	.global kheap_size
	.global kheap_end
	.global pheap_size
	.global pheap_end
	.global gdt_table
	.global procs_addr
	.global tss_sect
	.extern kheap_e
	.extern pheap_e
	.extern processes
    kstack_size: .int KSTACK_SZ
	kheap_size: .int KHEAP_SZ
	kheap_end: .int kheap_e
	pheap_size: .int PHEAP_SZ
	pheap_end: .int pheap_e
	gdt_table: .space GDTTB_SZ
	tss_sect:  .space TSS_SZ
	procs_addr: .long processes
	

/*
    Now that we have all of that sorted out,
    we can now define kernel's entry point.
*/
.section .text
    .extern init
	.extern main
	.extern on_error
    .global _start

    _start:
		cli

        // initialize the kernel stack pointer
        mov $kstack_top, %esp
		// set ebp
		mov $kstack_top, %ebp
        
        // now push the multiboot info onto the stack
        push %ebx
        call init

		// now we can call main
		call main

		// halt the kernel if main returns
		hlt

.section .end
	.global kend
	.global processes
	kend:
		hlt
	processes:
	