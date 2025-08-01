#ifndef MAP_H
#define MAP_H

// refs to PLM4 structures
extern uint64_t* pml4_table;
extern uint64_t* pdpt_table;
extern uint64_t* pd_table;
extern uint64_t* pt_tables;

// refs to sections
extern char _kernel_text_boot_start[];
extern char _kernel_text_boot_end[];

extern char _kernel_text_start[];
extern char _kernel_text_end[];

extern char _kernel_rodata_start[];
extern char _kernel_rodata_end[];

extern char _kernel_data_start[];
extern char _kernel_data_end[];

extern char _kernel_bss_start[];
extern char _kernel_bss_end[];

extern char _kernel_stack_start[];
extern char _kernel_stack_end[];

// PTE flags
#define PTE_PRESENT    (1ULL << 0)
#define PTE_WRITABLE   (1ULL << 1)
#define PTE_USER       (1ULL << 2)
#define PTE_NX         (1ULL << 63)

#define PAGE_SIZE      4096
#define ENTRIES_PER_PT 512

static void map_range (uint64_t virt_start, uint64_t phys_start, 
                       uint64_t size, uint64_t flags)
{
    uint64_t virt_end  = virt_start + size;
    uint64_t phys_addr = phys_start;
    
    // calculate PT index for the virtual address
    uint64_t pt_i = (virt_start >> 12) & 0x1FF;
    
    while (virt_start < virt_end)
    {
        // setup PTE
        uint64_t pte = phys_addr | flags | PTE_PRESENT;
        pt_tables[pt_i] = pte;
        
        virt_start += PAGE_SIZE;
        phys_addr  += PAGE_SIZE;
        pt_i++;
        
        // if reachs the end of current PT, break
        if (pt_i >= ENTRIES_PER_PT)
            break;
    }
}

void init_map_kernel_sections(void)
{
    map_range
    (
        (uint64_t)_kernel_text_boot_start,
        (uint64_t)_kernel_text_boot_start,
        (uint64_t)(_kernel_text_boot_end - _kernel_text_boot_start),
        PTE_PRESENT
    );

    // .text - read-only, executable
    map_range
    (
        (uint64_t)_kernel_text_start,
        (uint64_t)_kernel_text_start,
        (uint64_t)(_kernel_text_end - _kernel_text_start),
        PTE_PRESENT
    );
    *(uintptr_t*)(_kernel_text_start + (unsigned char)1);
    kprintf("[ BOOT ] text -> start %p ; end %p\n", _kernel_text_start, _kernel_text_end);
    
    // .rodata - read-only, non-executable
    map_range
    (
        (uint64_t)_kernel_rodata_start,
        (uint64_t)_kernel_rodata_start,
        (uint64_t)(_kernel_rodata_end - _kernel_rodata_start),
        PTE_PRESENT | PTE_NX
    );
    *(uintptr_t*)(_kernel_rodata_start + (unsigned char)1);
    
    // .data - read-write, non-executable
    map_range
    (
        (uint64_t)_kernel_data_start,
        (uint64_t)_kernel_data_start,
        (uint64_t)(_kernel_data_end - _kernel_data_start),
        PTE_PRESENT | PTE_WRITABLE | PTE_NX
    );
    *(uintptr_t*)(_kernel_data_start + (unsigned char)1);
    kprintf("[ BOOT ] data -> start %p ; end %p\n", _kernel_data_start, _kernel_data_end);
    
    // .bss - read-write, non-executable
    map_range
    (
        (uint64_t)_kernel_bss_start,
        (uint64_t)_kernel_bss_start,
        (uint64_t)(_kernel_bss_end - _kernel_bss_start),
        PTE_PRESENT | PTE_WRITABLE | PTE_NX
    );
    *(uintptr_t*)(_kernel_bss_start + (unsigned char)1);
    
    // .stack - read-write, non-executable
    map_range
    (
        (uint64_t)_kernel_stack_start,
        (uint64_t)_kernel_stack_start,
        (uint64_t)(_kernel_stack_end - _kernel_stack_start),
        PTE_PRESENT | PTE_WRITABLE | PTE_NX
    );
    *(uintptr_t*)(_kernel_stack_end - (unsigned char)1);
    kprintf("[ BOOT ] stack -> start %p ; end %p\n", _kernel_stack_start, _kernel_stack_end);
}

#endif
