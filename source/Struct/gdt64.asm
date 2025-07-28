; setting up GDT for 64 bit mode
GDT64:
    .null: equ $ - GDT64
        dq 0
    .code: equ $ - GDT64
        dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) | (1 << 41)
    .data: equ $ - GDT64
        dq (1 << 44) | (1 << 47) | (1 << 41)
    .descriptor:
        dw $ - GDT64 - 1
        dq GDT64

    .code_ptr equ 0x08
    .data_ptr equ 0x10
