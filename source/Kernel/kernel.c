// 0xffffff8000000000 - 0xffffff8000200000 [pt_ffffff8000000]
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "modules/macro.h"
#include "modules/prototype.h"

// self
volatile uint64_t cpu_ticks = 0;

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

    init_idt();
    init_map_kernel_sections();

    // RIP cant handle canonical address yet -> use trampolines (avoid RIP + offset)
    asm volatile
    (
        "movabs %[target], %%rax\n\t"
        "jmpq *%%rax\n\t"
        :
        : [target] "i"(init)
        : "rax"
    );

    __builtin_unreachable();
}

#include "modules/wrapper.h"
#include "modules/tty.h"

__attribute__((noreturn)) void panic(void)
{
    asm volatile("cli\n\t");

    kprintf("kernel panic!\n");

    halt();
}

#include "modules/init.h"
// #include "modules/loader.h"

void sleep(uint64_t ms)
{
    uint64_t target = cpu_ticks + ms / 10;

    while (cpu_ticks < target)
    {
        asm volatile("hlt");
    }
}

void main(void)
{
    kprintf("\nv0 is alive!\n");

    halt();
}
