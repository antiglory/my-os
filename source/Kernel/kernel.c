#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include "modules/macro.h"

BTEXT ALIGNED void kstart(uint64_t* pml4, uint64_t* pdpt, uint64_t* pd, uint64_t* pt)  __attribute__((naked));
TEXT  ALIGNED void setup_kernel_paging(void);
TEXT  ALIGNED void init(void);
TEXT  ALIGNED void main(void);

extern char __stack_start[];
extern char __stack_end[];

uint64_t* pt_tables;
uint64_t* pd_table;
uint64_t* pdpt_table;
uint64_t* pml4_table;

// kernel entrypoint
void kstart(uint64_t* ext_pml4, uint64_t* ext_pdpt, uint64_t* ext_pd, uint64_t* ext_pt)
{
    asm volatile
    (
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
    );

    pml4_table = ext_pml4;
    pdpt_table = ext_pdpt;
    pd_table   = ext_pd;
    pt_tables  = ext_pt;

    asm volatile
    (
        // setup kernel stack
        "movq %0, %%rax\n\t"
        "movq %%rax, %%rsp\n\t"
        "movq %%rsp, %%rbp\n\t"

        :
        : "r"(__stack_end)
        : "rax", "memory"
    );

    setup_kernel_paging();

    asm volatile
    (
        "movabs %[target], %%rax\n\t"
        "jmp *%%rax\n\t"
        :
        : [target] "i"(init)
        : "rax"
    );

    __builtin_unreachable();
}

#include "modules/wrappers.h"
#include "modules/io.h"
#include "modules/init.h"
// #include "modules/loader.h"

void init(void) // 0x10058e
{
    // 0xffffff8000000000 - 0xffffff8000200000 [pt_ffffff8000000]

    idt_init();

    JMP(&main);
}

void main(void)
{
    kprintf("Greetz! - v0.1\n");

    halt();
}
