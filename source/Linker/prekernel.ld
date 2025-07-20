ENTRY(pstart)
SECTIONS
{
    . = 0x1000;

    .text : {
        *(.text.start)
        *(.text)
    }

    .text.start : { *(.text.start) }

    . = ALIGN(512);

    __prekernel_end = .;
}
