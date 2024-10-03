[BITS 16]
[ORG 0x7C00]

PREKERNEL_ENTRY equ 0x1000

mov [BOOT_DISK], dl

_start:
    cli

    ; segment regs
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; stack (but i'll not use in this label)
    mov bp, 0x7C00
    mov sp, bp

    ; loading prekernel into block 2
    mov bx, PREKERNEL_ENTRY
    mov dh, 2                  ; reading only 2 blocks
    mov ah, 2
    mov al, dh
    mov ch, 0
    mov dh, 0
    mov cl, 2
    mov dl, [BOOT_DISK]
    int 0x13

    mov ah, 0
    mov al, 3
    int 0x10

    ; setup GDT
    lgdt [GDT32.descriptor]

    ; enabling A20 line (to read more than 1MB of mem)
    in al, 0x92
    or al, 2
    out 0x92, al

    ; switch to protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; jump to protected mode
    jmp GDT32.code_ptr:protected_mode

%include "source/Struct/gdt32.asm"

[BITS 32]
protected_mode:
    mov ax, GDT32.data_ptr
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov bp, 0x1000
    mov sp, bp

    jmp PREKERNEL_ENTRY

BOOT_DISK: db 0

times 510-($-$$) db 0
dw 0xAA55
