#include "../types.h"
#include "../config.h"
#include "../Struct/gdt64.h"

struct gdt_entry gdt[3] __attribute__((aligned(8)));

void setup_gdt(void) {
    def64gdt(gdt);

    uint64_t gdt_d[2]; // gdt_descriptor

    gdt_d[0] = (sizeof(gdt) - 1) | ((uint64_t)gdt << 16);
    gdt_d[1] = ((uint64_t)gdt >> 48);

    asm volatile(
        "lgdt (%0)"
        :
        : "r"(gdt_d)
    );
}

#ifdef PLM4
    #define PML4_ENTRY_COUNT  512
    #define PDPTE_ENTRY_COUNT 512
    #define PD_ENTRY_COUNT    512
    #define PT_ENTRY_COUNT    512

    struct pml4_entry {
        uint64_t present   : 1;
        uint64_t rw        : 1;
        uint64_t us        : 1;
        uint64_t pwt       : 1;
        uint64_t pcd       : 1;
        uint64_t accessed  : 1;
        uint64_t ignored_1 : 1;
        uint64_t mbz       : 1;
        uint64_t ignored_2 : 4;
        uint64_t addr      : 40;
        uint64_t ignored_3 : 11;
        uint64_t nx        : 1;
    };

    struct pml4_entry pml4  [PML4_ENTRY_COUNT] __attribute__((aligned(4096)));
    struct pml4_entry pdpte [PDPTE_ENTRY_COUNT] __attribute__((aligned(4096)));
    struct pml4_entry pd    [PD_ENTRY_COUNT]    __attribute__((aligned(4096)));
    struct pml4_entry pt    [PT_ENTRY_COUNT]    __attribute__((aligned(4096)));

    void setup_paging() {
        pml4[0].present = 1;
        pml4[0].rw = 1;
        pml4[0].addr = (uint64_t) &pdpte >> 12;

        pdpte[0].present = 1;
        pdpte[0].rw = 1;
        pdpte[0].addr = (uint64_t) &pd >> 12;

        pd[0].present = 1;
        pd[0].rw = 1;
        pd[0].addr = (uint64_t) &pt >> 12;

        pt[0].present = 1;
        pt[0].rw = 1;
        pt[0].addr = 0;

        uint64_t cr3_value = (uint64_t) &pml4;

        asm volatile(
            "mov %0, %%cr3"
            :
            : "r" (cr3_value)
        );
    }
#endif

void long_mode(void) {
    asm volatile(
        "mov %%cr4, %%rax       \n\t"
        "mov $0x20, %%rdi       \n\t"
        "or %%rdi, %%rax        \n\t"
        "mov %%rax, %%cr4       \n\t"
        "mov %%cr0, %%rax       \n\t"
        "mov $0x80000001, %%rdi \n\t"
        "or %%rdi, %%rax        \n\t"
        "mov %%rax, %%cr0       \n\t"
        :
        :
        : "rax", "rdi"
    );

    asm volatile(
        "mov $0x10, %%ax                   \n\t"  // data segment selector
        "mov %%ax, %%ds                    \n\t"
        "mov %%ax, %%es                    \n\t"
        "mov %%ax, %%ss                    \n\t"
        "pushq $0x08                       \n\t"
        "lea long_mode_entry(%%rip), %%rax \n\t"
        "pushq %%rax                       \n\t"  // push target offset
        "retfq                             \n\t"  // far return to the long_mode_entry label
        "long_mode_entry:                  \n\t"
        "mov $0x18, %%ax                   \n\t"  // load kernel data segment selector
        "mov %%ax, %%ds                    \n\t"
        "mov %%ax, %%es                    \n\t"
        "mov %%ax, %%ss                    \n\t"
        :
        :
        : "rax"
    );
}

void main() {
    setup_gdt();
    setup_paging();
    long_mode();
}
