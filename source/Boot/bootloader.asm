; -> bootloader loada o prekernel | OK
; -> prekernel inicializa a GDT e a IDT
; -> prekernel entra no modo protegido
; -> prekernel carrega o kernel na memoria em endereços altos pq ta no modo protegido (precisa criar a propria funcao pra mandar informacoes pro disco, como se fosse um pequeno driver, pq n tem mais BIOS interrupt)
; -> prekernel carrega modo longo com uma paginação provisória (em certo modo, uma unica entrada na PML4 pra abranger apenas os primeiros 1GB de memoria, e tudo self-paged, ou seja, endereço fisico = endereço virtual, pra facilitar)
; -> ai jumpa pro kernel e a partir de lá, em C, consigo criar uma paginação completa pro kernel e posteriormente PML4 de cada processo q vai ser criado

[BITS 16]
[ORG 0x7C00]

_start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; prekernel loading
    mov ah, 0x02      ; disk read function ID
    mov al, 1         ; block num to read
    mov ch, 0         ; cilinder 0
    mov cl, 2         ; block 2 (prekernel) (bootloader is at block 1)
    mov dh, 0         ; head 0
    mov dl, 0x80      ; drive 0x80 (first hard disk)
    mov bx, 0x1000    ; segment which prekernel will be loaded
    mov es, bx
    xor bx, bx        ; offset 0 inside segment
    int 0x13
    jc disk_error

    mov si, PREKERNEL_INFO_SUCC_LOADED
    call ssprintnf

    mov ax, 0x1000
    mov ds, ax
    mov es, ax

    jmp 0x1000:0x0000

; system simple print not formatted
; arguments:
;   SI = pointer to the string
ssprintnf:
    lodsb
    or al, al
    jz .d
    mov ah, 0x0E
    int 0x10
    jmp ssprintnf
.d:
    ret

disk_error:
    sti

    mov si, DISK_ERROR_LOAD
    call ssprintnf

    xor si, si

    mov si, DISK_ERROR_HALT
    call ssprintnf

    hlt

DISK_ERROR_LOAD            db "[ ERROR ] cannot load prekernel at block 2",           0x0D, 0x0A, 0x0
DISK_ERROR_HALT            db "[ WARN ] no IDT has been initialized, boot was trunk", 0X0D, 0X0A, 0x0

PREKERNEL_INFO_SUCC_LOADED db "[ INFO ] sucessfully loaded prekernel at block 2",     0x0D, 0x0A, 0x0

times 510-($-$$) db 0
dw 0xAA55
