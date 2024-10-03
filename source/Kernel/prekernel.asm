global start

start:
    ; state:
    ; running at 0x1000

    ; setup page table
    call setup_paging

    ; enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; enable PSE
    mov eax, cr4
    or eax, 1 << 4 
    mov cr4, eax

    ; clear CD and NW in CR0
    mov eax, cr0
    and eax, 0x9FFFFFFF
    mov cr0, eax

    ; set CR3 to point to the PML4 table
    mov eax, pml4_table
    mov cr3, eax

    ; enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging and load GDT
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [GDT64.descriptor]

    jmp GDT64.code_ptr:end

setup_paging:
    ; state:
    ; running at 0x1051

    mov edi, pml4_table
    mov eax, pdpt_table
    and eax, 0xFFFFF000 ; zeroing reserved bits
    or eax, PTE_PRESENT | PTE_WRITABLE | PTE_USER
    mov ecx, 512  ; entry num in PML4
.setup_pml4:
    mov [edi], eax
    add edi, 8
    loop .setup_pml4

    mov edi, pdpt_table
    mov eax, pd_table
    and eax, 0xFFFFF000 ; zeroing reserved bits
    or eax, PTE_PRESENT | PTE_WRITABLE | PTE_USER
    mov ecx, 512  ; entry num in PDPT
.setup_pdpt:
    mov [edi], eax
    add edi, 8
    loop .setup_pdpt

    mov edi, pd_table
    mov eax, pt_tables
    or eax, PTE_PRESENT | PTE_WRITABLE | PTE_USER
    mov ecx, 512  ; entry num in PD
.setup_pd:
    ; PTs (128GB map)
    mov edi, pt_tables
    xor eax, eax ; starting from physical address 0x0
    mov ecx, 65536  ; 128 * 512 entry
.setup_pt:
    and eax, 0xFFFFF000 ; zeroing reserved bits
    or eax, PTE_PRESENT | PTE_WRITABLE | PTE_USER
    mov [edi], eax
    add edi, 8
    add eax, PAGE_SIZE
    loop .setup_pt

KERNEL_PHYSICAL_ADDRESS equ 0x100000
KERNEL_VIRTUAL_ADDRESS  equ 0xFFFFFFFF80100000

KERNEL_SECTOR_START equ 8
KERNEL_SECTOR_COUNT equ 8

[BITS 64]
; 0x10e1
end:
    call load_kernel

    mov rax, KERNEL_PHYSICAL_ADDRESS
    jmp rax

; 0x10f3
load_kernel:
    mov edi, KERNEL_PHYSICAL_ADDRESS  ; target address
    mov eax, KERNEL_SECTOR_START      ; starting block
    mov ecx, KERNEL_SECTOR_COUNT      ; block num to read
.read_loop:
    push rcx
    push rax

    ; select the primary bus
    mov dx, 0x1F6
    mov al, 0xE0  ; LBA mode, primary drive
    out dx, al

    ; set sector count
    mov dx, 0x1F2
    mov al, 1
    out dx, al

    ; send LBA
    mov dx, 0x1F3
    mov al, cl
    out dx, al

    inc dx  ; 0x1F4
    mov al, ch
    out dx, al

    inc dx  ; 0x1F5
    shr eax, 16
    out dx, al

    ; send read command
    mov dx, 0x1F7
    mov al, 0x20  ; read sectors command
    out dx, al

    ; wait for the disk (with timeout)
    mov ecx, 100000  ; timeout counter
.wait_disk:
    in al, dx
    test al, 8  ; check if BSY (bit 7) is clear and DRQ (bit 3) is set
    jnz .ready
    loop .wait_disk

    ; if we get here, weve timed out
    mov rdi, DISK_ERROR
    call pkprintnf

    hlt
.ready:
    ; read 256 words
    mov ecx, 256
    mov dx, 0x1F0
    rep insw

    pop rax
    inc eax
    pop rcx
    loop .read_loop

    ret

; rdi: string address
pkprintnf:
    mov rsi, rdi
    mov rbx, 0xB8000    ; video mem address to text mode
    mov rcx, 0          ; char index (shift in video mem)
.l:
    lodsb               ; loads the next byte of the string in al (inc rsi)
    test al, al
    jz .e               ; if 0, end

    ; write char in video mem
    mov [rbx + rcx*2], al               ; char (each cell have 2 bytes for char and color)
    mov BYTE [rbx + rcx*2 + 1], 0x07    ; white color, black back

    inc rcx     ; next position in video mem
    jmp .l      ; loops to the next char
.e:
    ret

DISK_ERROR            db "[ ERROR ] disk read for kernel timeout",          0x0

; 0x1e4 (484 bytes from start)
%include "source/Struct/gdt64.asm"

PAGE_SIZE         equ 4096
ENTRIES_PER_TABLE equ 512
PTE_PRESENT       equ 1
PTE_WRITABLE      equ 2
PTE_USER          equ 4

align 4096
pml4_table resb PAGE_SIZE
pdpt_table resb PAGE_SIZE
pd_table   resb PAGE_SIZE
pt_tables  resb 512 * PAGE_SIZE
