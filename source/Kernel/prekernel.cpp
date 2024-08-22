#include <stdint.h>

#include "../config.h"
#include "modules/io.cpp"

// i think GDT for long mode is missing uhdasuhdasuhdhua

struct PageMapLevel4Entry {
    uint64_t present            : 1;
    uint64_t writable           : 1;
    uint64_t user_accessible    : 1;
    uint64_t write_through      : 1;
    uint64_t cache_disabled     : 1;
    uint64_t accessed           : 1;
    uint64_t ignored_1          : 1;
    uint64_t page_size          : 1;
    uint64_t ignored_2          : 4;
    uint64_t physical_address   : 40;
    uint64_t ignored_3          : 11;
    uint64_t execution_disabled : 1;
} __attribute__((packed));

void setup_paging() {
    // allocate memory for PLM4
    PageMapLevel4Entry* pml4 = (PageMapLevel4Entry*)0x1000;
    
    // setup the first entry to map the first 1GB
    pml4[0].present = 1;
    pml4[0].writable = 1;
    pml4[0].physical_address = 0x2000 >> 12;

    // setup PDPT
    uint64_t* pdpt = (uint64_t*)0x2000;
    pdpt[0] = 0x83; // present, writable, huge page (1GB)

    // flush TLB
    asm volatile (
        "mov %0, %%cr3" 
        : 
        : "r" ((uint32_t)pml4)
    );
}

void enable_long_mode() {
    uint32_t eax, edx;

    // verify if hardware supports long mode
    asm volatile (
        "movl $0x80000001, %%eax\n\t"
        "cpuid\n\t"
        "testl $0x20000000, %%edx\n\t"
        "jz no_long_mode"
        :
        :
        : "eax", "ebx", "ecx", "edx"
    );

    // turns on PAE
    asm volatile (
        "movl %%cr4, %%eax\n\t"
        "orl $0x20, %%eax\n\t"
        "movl %%eax, %%cr4" 
        : 
        : 
        : "eax"
    );

    // loads CR3 with PML4 address
    asm volatile (
        "movl $0x1000, %%eax\n\t"
        "movl %%eax, %%cr3"
        :
        :
        : "eax"
    );

    // turns on long mode in EFER MSR
    asm volatile (
        "movl $0xC0000080, %%ecx\n\t"
        "rdmsr\n\t"
        "orl $0x100, %%eax\n\t"
        "wrmsr" 
        : 
        : 
        : "eax", "ecx", "edx"
    );

    // turns pagination on
    asm volatile (
        "movl %%cr0, %%eax\n\t"
        "orl $0x80000000, %%eax\n\t"
        "hlt\n\t"
        "movl %%eax, %%cr0" // fault here, TODO
        : 
        : 
        : "eax"
    );

    // outpoint
    asm volatile ("no_long_mode:");
}

extern "C" void main(void) {
    // state:
    // ESP and EBP are at 0x1000;
    // 32 bits/protected mode - ring 0

    const char PK_ENTRYP_PK_REACHED[] = "[ INFO ] prekernel was reached\0";
    const char PK_ENTRYP_LM_LOADED[]  = "[ INFO ] long mode was loaded\0";

    clear_screen();
    pkprintnf(PK_ENTRYP_PK_REACHED);

    setup_paging();
    enable_long_mode();

    pkprintnf(PK_ENTRYP_LM_LOADED);

    while (1);
}
