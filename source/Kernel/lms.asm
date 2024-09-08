[BITS 32]
global LMS

LMS:
    ; setup page table
    call setup_paging

    ; PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax    ; fault here :(

    lgdt [GDT64.descriptor]

    jmp GDT64.code_ptr:long_mode_start

setup_paging:
    ; setup PLM4
    mov eax, pdpt_table
    or eax, 0b11 ; presence + writable
    mov [pml4_table], eax
    
    ; setup PDPT
    mov eax, pd_table
    or eax, 0b11 ; presence + writable
    mov [pdpt_table], eax
    
    ; setup PD
    mov eax, pt_table
    or eax, 0b11 ; presence + writable
    mov [pd_table], eax
    
    ; setup PT (map first 2MB)
    mov ecx, 0 ; counter
.map_pt_table:
    mov eax, 0x1000 ; 4KB page
    mul ecx
    or eax, 0b11 ; presence + writable
    mov [pt_table + ecx * 8], eax
    inc ecx
    cmp ecx, 512 ; table entries
    jne .map_pt_table
    
    ; load PML4 in CR3
    mov eax, pml4_table
    mov cr3, eax
    
    ret

[BITS 64]
long_mode_start:
    hlt ; eureka?!

; page table
align 4096
pml4_table:
    times 512 dq 0
align 4096
pdpt_table:
    times 512 dq 0
align 4096
pd_table:
    times 512 dq 0
align 4096
pt_table:
    times 512 dq 0

GDT64:
    .null: equ $ - GDT64
        dq 0
    .code: equ $ - GDT64
        dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
    .data: equ $ - GDT64
        dq (1 << 44) | (1 << 47) | (1 << 41)
    .descriptor:
        dw $ - GDT64 - 1
        dq GDT64

    .code_ptr equ GDT64.code - GDT64
    .data_ptr equ GDT64.data - GDT64

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
