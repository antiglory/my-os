#ifndef MAP_H
#define MAP_H

// External references to page table structures
extern uint64_t* pml4_table;
extern uint64_t* pdpt_table;
extern uint64_t* pd_table;
extern uint64_t* pt_tables;

// External references to sections
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

// Page table entry flags
#define PTE_PRESENT    (1ULL << 0)
#define PTE_WRITABLE   (1ULL << 1)
#define PTE_USER       (1ULL << 2)
#define PTE_NX         (1ULL << 63)

#define PAGE_SIZE      4096
#define ENTRIES_PER_PT 512

// Function to map a virtual range to physical range with specific flags
static void map_range (uint64_t virt_start, uint64_t phys_start, 
                       uint64_t size, uint64_t flags)
{
    uint64_t virt_end  = virt_start + size;
    uint64_t phys_addr = phys_start;
    
    // Calculate PT index for the virtual address
    uint64_t pt_idx = (virt_start >> 12) & 0x1FF;
    
    while (virt_start < virt_end)
    {
        // Create PTE
        uint64_t pte = phys_addr | flags | PTE_PRESENT;
        pt_tables[pt_idx] = pte;
        
        virt_start += PAGE_SIZE;
        phys_addr  += PAGE_SIZE;
        pt_idx++;
        
        // If we reach the end of current PT, break
        if (pt_idx >= ENTRIES_PER_PT)
            break;
    }
}

void setup_kernel_paging(void)
{
    // Map text section (code) - read-only, executable
    map_range(
        (uint64_t)_kernel_text_start,
        (uint64_t)_kernel_text_start,
        (uint64_t)(_kernel_text_end - _kernel_text_start),
        PTE_PRESENT
    );
    
    // Map rodata section - read-only, non-executable
    map_range(
        (uint64_t)_kernel_rodata_start,
        (uint64_t)_kernel_rodata_start,
        (uint64_t)(_kernel_rodata_end - _kernel_rodata_start),
        PTE_PRESENT | PTE_NX
    );
    
    // Map data section - read-write, non-executable
    map_range(
        (uint64_t)_kernel_data_start,
        (uint64_t)_kernel_data_start,
        (uint64_t)(_kernel_data_end - _kernel_data_start),
        PTE_PRESENT | PTE_WRITABLE | PTE_NX
    );
    
    // Map bss section - read-write, non-executable
    map_range(
        (uint64_t)_kernel_bss_start,
        (uint64_t)_kernel_bss_start,
        (uint64_t)(_kernel_bss_end - _kernel_bss_start),
        PTE_PRESENT | PTE_WRITABLE | PTE_NX
    );
    
    // Map stack - read-write, non-executable
    map_range(
        (uint64_t)_kernel_stack_start,
        (uint64_t)_kernel_stack_start,
        (uint64_t)(_kernel_stack_end - _kernel_stack_start),
        PTE_PRESENT | PTE_WRITABLE | PTE_NX
    );
}

#endif