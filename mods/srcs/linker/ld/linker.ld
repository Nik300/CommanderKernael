ENTRY(_start)

SECTIONS
{
    . = 0x8048000;

    .data BLOCK(4K) : ALIGN(4K)
    {
		*(.data)
        *(.data.*)
    }
    .rodata BLOCK(4K) : ALIGN(4K)
    {
		*(.rodata)
        *(.rodata.*)
    }
    .bss BLOCK(4K) : ALIGN(4K)
    {
		*(.bss)
        *(.bss.*)
    }
    .text BLOCK(4K) : ALIGN(4K)
    {
		*(.text.start)
		*(.text)
        *(.text.*)
    }
}