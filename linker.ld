ENTRY(main)

SECTIONS
{
    . = 0x100000;
    .text.entry : { *(.text.entry) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}