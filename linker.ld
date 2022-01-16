ENTRY(_start)

SECTIONS
{
    . = 1M;

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.data)
    }
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(.bss)
    }
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)
		*(.end)
    }
}