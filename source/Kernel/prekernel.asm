[BITS 16]
[ORG 0x0000]

prekernel:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0xFFFE

    mov si, PREKERNEL_INFO_REACHED
    call ssprintnf

    lgdt [gdt_d]

    ; enabling protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    hlt ; for now, im halting before protected mode far jmp because it is having some issues

    ; far jump pro protected mode (far pra limpar a pipeline da CPU)
    jmp CODE_S:protected_mode

[BITS 32]
protected_mode:
    ; setando os segmentos
    mov ax, DATA_S
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000 ; (stack)

    hlt

; simple system print not formatted (duplicated)
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

PREKERNEL_INFO_REACHED db "[ INFO ] prekernel was reached", 0x0D, 0x0A, 0x0

; GDT
gdt_start:
    dq 0    ; null selector

gdt_code:
    dw 0xFFFF       ; limit (bits 0-15) (64kb - 1 byte)
    dw 0            ; base (bits 0-15) (segmento começa no address 0)
    db 0            ; base (bits 16-23) (segmento começa no address 0)
    db 10011010b    ; access flags (segmento presente, ring 0, bd 1, tipo 0, direção 0, wp 1, ab 0)
    db 11001111b    ; limit & flags (bits 16-19) (granularidade 1, op size 1, always 0, limite 1111)
    db 0            ; base (bits 24-31) (completa o base address)

gdt_data:
    dw 0xFFFF       ; limit (bits 0-15) (64kb - 1 byte)
    dw 0            ; base (bits 0-15) (segmento começa no address 0)
    db 0            ; base (bits 16-23) (segmento começa no address 0)
    db 10010010b    ; access flags (segmento presente, ring 0, bd 1, tipo 0, direção 0, wp 1, ab 0)
    db 11001111b    ; limit & flags (bits 16-19) (granularidade 1, op size 1, always 0, limit 1111)
    db 0            ; base (bits 24-31) (completa o address)

gdt_end:
    ; null

gdt_d:
    dw gdt_end - gdt_start - 1  ; tamanho da GDT
    dd gdt_start                ; address da GDT

CODE_S equ gdt_code - gdt_start
DATA_S equ gdt_data - gdt_start