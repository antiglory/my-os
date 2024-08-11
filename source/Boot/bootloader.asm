; -> bootloader inicializa a GDT pro modo protegido | OK
; -> bootlodar entra no modo protegido
; -> bootloader carrega o prekernel na .text da gdt32
; -> prekernel carrega o kernel na memoria em endereços altos pq ta no modo protegido (precisa criar a propria funcao pra mandar informacoes pro disco, como se fosse um pequeno driver, pq n tem mais BIOS interrupt)
; -> prekernel carrega o modo longo com uma paginação provisória (em certo modo, uma unica entrada na PML4 pra abranger apenas os primeiros 1GB de memoria, e tudo self-paged, ou seja, endereço fisico = endereço virtual, pra facilitar)
; -> ai jumpa pro kernel e a partir de lá, em C, consigo criar uma paginação completa pro kernel e posteriormente PML4 de cada processo q vai ser criado

; enable A20 line
; in al, 0x92
; or al, 2
; out 0x92, al

; prekernel loading
; mov ah, 0x02      ; disk read function ID
; mov al, 2         ; blocks to read
; mov ch, 0         ; cylinder 0
; mov cl, 2         ; block 2 (prekernel) (bootloader is at block 1)
; mov dh, 0         ; head 0
; mov dl, 0x80      ; drive 0x80 (first hard disk)
; mov bx, 0x1000    ; segment which prekernel will be loaded
; mov es, bx
; xor bx, bx        ; offset 0 inside segment
; int 0x13
; jc disk_error
; mov si, PREKERNEL_INFO_SUCC_LOADED
; call ssprintnf

; disk_error:
;     sti
;
;     mov si, DISK_ERROR_LOAD
;     call ssprintnf
;
;     xor si, si
; 
;     mov si, DISK_ERROR_HALT
;     call ssprintnf
;
;    hlt

; DISK_ERROR_LOAD            db "[ ERROR ] cannot load prekernel at block 2",            0x0D, 0x0A, 0x0
; DISK_ERROR_HALT            db "[ WARN ] no IDT has been initialized, boot has been trunk",  0X0D, 0X0A, 0x0

%include "../Struct/gdt32.asm"
[BITS 16]
[ORG 0x7C00]

_start:
    cli

    ; segment regs
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; stack
    mov bp, 0x7C00
    mov sp, bp

    call bmmap
    lgdt [GDT32.ptr]
    
    mov si, ENTRYP_INFO_LOADED_GDT
    call ssprintnf

    ; enabling A20 line (to read more than 1MB of mem)
    in al, 0x92
    or al, 2
    out 0x92, al

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; far jump to protected mode
    ; triple fault here
    jmp GDT32.code:protected_mode

[BITS 32]
protected_mode:
    mov ax, GDT32.data
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000 ; (stack)

    mov si, PM_INFO_ALIVE
    call ssprintnf

    hlt

bmmap:
    pusha

    mov ax, 0xE820
    xor bx, bx
    mov bp, 0
.loop:
    db 0x66

    mov ecx, 0x534D4150 ; SMAP
    add di, bp  
    mov di, 0x500 ; memory addr; will be used in Kernel
    add bp, 20 ; 20 = data size
    int 0x15

    jc .error

    cmp bx, 0
    jz .finish
    jmp .loop
.finish:
    popa
    ret
.error:

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

ENTRYP_INFO_LOADED_GDT     db "[ INFO ] 32-bit GDT is alive",               0x0D, 0x0A, 0x0

PM_INFO_ALIVE              db "[ INFO ] EUREKA!",   0x0D, 0x0A, 0x0

PREKERNEL_INFO_SUCC_LOADED db "[ INFO ] sucessfully loaded prekernel at block 2",  0x0D, 0x0A, 0x0

times 510-($-$$) db 0
dw 0xAA55
