; -> bootloader inicializa a GDT pro modo protegido | OK
; -> bootloader carrega o prekernel | OK
; -> bootlodar entra no modo protegido | OK
; -> bootloader jumpa pro prekernel | OK
; -> prekernel carrega o kernel na memoria em endereços altos pq ta no modo protegido (precisa criar a propria funcao pra mandar informacoes pro disco, como se fosse um pequeno driver, pq n tem mais BIOS interrupt)
; -> prekernel carrega o modo longo com uma paginação provisória (em certo modo, uma unica entrada na PML4 pra abranger apenas os primeiros 1GB de memoria, e tudo self-paged, ou seja, endereço fisico = endereço virtual, pra facilitar)
; -> ai jumpa pro kernel e a partir de lá, em C, consigo criar uma paginação completa pro kernel e posteriormente PML4 de cada processo q vai ser criado

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
    mov dh, 2
    mov ah, 0x02
    mov al, dh 
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    mov dl, [BOOT_DISK]     ; no disk error handler, TODO
    int 0x13        

    mov ah, 0x0
    mov al, 0x3
    int 0x10                ; text mode

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
