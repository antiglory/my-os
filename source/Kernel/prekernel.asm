; _s: selector
; _d: descriptor

[BITS 16]

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

[BITS 32]

    ; far jump to protected mode (far to clean CPU pipeline)
    jmp CODE_S:protected_mode

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

[BITS 16]

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

; GDT
gdt_start:
    dq 0x0          ; null selector
gdt_code:
    dw 0xFFFF       ; limit (bits 0-15) (64kb - 1 byte)
    dw 0x0          ; base (bits 0-15) (segment start at address 0)
    db 0x0          ; base (bits 16-23) (segment start at address 0)
    db 10011010b    ; access flags (present segment, ring 0, bd 1, type 0, direction 0, wp 1, ab 0)
    db 11001111b    ; limit & flags (bits 16-19) (granularity 1, op size 1, always 0, limit 1111)
    db 0x0          ; base (bits 24-31) (fill the base address)
gdt_data:
    dw 0xFFFF       ; limit (bits 0-15) (64kb - 1 byte)
    dw 0x0          ; base (bits 0-15) (segment start at address 0)
    db 0x0          ; base (bits 16-23) (segment start at address 0)
    db 10010010b    ; access flags (present segment, ring 0, bd 1, type 0, direction 0, wp 1, ab 0)
    db 11001111b    ; limit & flags (bits 16-19) (granularity 1, op size 1, always 0, limit 1111)
    db 0x0          ; base (bits 24-31) (fill the address)
gdt_end:

gdt_d:
    dw gdt_end - gdt_start      ; GDT size
    dd gdt_start                ; GDT address

CODE_S equ gdt_code - gdt_start
DATA_S equ gdt_data - gdt_start

PREKERNEL_INFO_REACHED db "[ INFO ] prekernel was reached", 0x0D, 0x0A, 0x0
