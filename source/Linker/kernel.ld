OUTPUT_FORMAT(binary)
ENTRY(kstart)

KERNEL_VMA = 0xFFFFFF8000100000;
KERNEL_LMA = 0x100000;

SECTIONS
{
    . = KERNEL_VMA;

    .text.boot : AT(KERNEL_VMA)
    {
        KEEP(*(kstart))
        *(.text.boot)
    }

    .text ALIGN(8) : AT(ADDR(.text))
    {
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata))
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data))
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss))
    {
        *(COMMON)
        *(.bss)
    }

    .stack ALIGN(4K) : AT(ADDR(.stack))
    {
        . = ALIGN(16);
        __stack_start = .;
        . = . + 0x1000;
        __stack_end = .;
    }

    /* exporting for kernel.c visibility */
    _kernel_text_start     = ADDR(.text);
    _kernel_text_end       = ADDR(.rodata);

    _kernel_rodata_start   = ADDR(.rodata);
    _kernel_rodata_end     = ADDR(.data);

    _kernel_data_start     = ADDR(.data);
    _kernel_data_end       = ADDR(.bss);

    _kernel_bss_start      = ADDR(.bss);
    _kernel_bss_end        = ADDR(.stack);

    _kernel_stack_start    = __stack_start;
    _kernel_stack_end      = __stack_end;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}
