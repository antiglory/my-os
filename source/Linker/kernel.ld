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

    .text ALIGN(0x100) : AT(ADDR(.text))
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
        . = . + 0x1000;
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
    _kernel_text_boot_start = ADDR(.text.boot);
    _kernel_text_boot_end   = ADDR(.text);

    _kernel_text_start      = ADDR(.text);
    _kernel_text_end        = ADDR(.rodata);

    _kernel_rodata_start    = ADDR(.rodata);
    _kernel_rodata_end      = ADDR(.data);

    _kernel_data_start      = ADDR(.data);
    _kernel_data_end        = ADDR(.bss);

    _kernel_bss_start       = ADDR(.bss);
    _kernel_bss_end         = ADDR(.stack);

    _kernel_stack_start     = __stack_start;
    _kernel_stack_end       = __stack_end;

    _kernel_vma             = KERNEL_VMA;
    _kernel_lma             = KERNEL_LMA;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}
