OUTPUT_FORMAT(binary)
ENTRY(_start)

KERNEL_VMA = 0xFFFFFFFF80100000;
KERNEL_LMA = 0x100000;

SECTIONS
{
    . = KERNEL_LMA;

    .start : AT(ADDR(.start))
    {
        KEEP(*(_start))
    }

    .text.boot : AT(ADDR(.text.boot))
    {
        *(.text.boot)
    }

    .text : AT(ADDR(.text))
    {
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA + KERNEL_LMA)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA + KERNEL_LMA)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA + KERNEL_LMA)
    {
        *(COMMON)
        *(.bss)
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}
