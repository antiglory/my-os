ENTRY(start)
SECTIONS
{
    . = 0x1000;

    .text : {
        *(.text.start)
        *(.text)
    }

    .text.start : { *(.text.start) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }

    . = ALIGN(512);

    __prekernel_end = .;
}
