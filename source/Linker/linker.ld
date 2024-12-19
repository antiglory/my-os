OUTPUT_FORMAT(binary)
ENTRY(_kstart)

KERNEL_VMA = 0xFFFF800000000000;
KERNEL_LMA = 0x100000;

SECTIONS
{
    . = KERNEL_LMA;

    .start : AT(ADDR(.start))
    {
        KEEP(*(_kstart))
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

    .stack ALIGN(4K) : AT(ADDR(.stack) - KERNEL_VMA + KERNEL_LMA)
    {
        . = ALIGN(16);
        __stack_start = .;
        . = . + 0x1000;  /* 4KB */
        __stack_end = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}
