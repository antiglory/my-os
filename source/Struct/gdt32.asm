; setting up GDT for 32 BIT MODE
GDT32:
    .null:
        dd 0x0, 0x0

    .code:
        dw 0xffff, 0x0
        db 0x0, 0b10011010, 0b11001111, 0x0

    .data:
        dw 0xffff, 0x0
        db 0x0, 0b10010010, 0b11001111, 0x0

    .descriptor:
        dw $ - GDT32 - 1
        dd GDT32

    .code_ptr equ GDT32.code - GDT32
    .data_ptr equ GDT32.data - GDT32
