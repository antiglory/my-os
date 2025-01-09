#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#define TEXT    __attribute__((section(".text")))
#define BTEXT   __attribute__((section(".text.boot")))
#define ALIGNED __attribute__((aligned(1)))

BTEXT ALIGNED void kstart()  __attribute__((naked));
TEXT  ALIGNED void init();

extern char __stack_start[];
extern char __stack_end[];

// kernel entrypoint
void kstart(void)
{
    // setup stack
    asm volatile
    (
        "cli\n\t"
        "movq %0, %%rax\n\t"
        "movq %%rax, %%rsp\n\t"
        "movq %%rsp, %%rbp\n\t"

        "jmp init\n\t"
        :
        : "r"(__stack_end)
        : "rax", "memory"
    );

    __builtin_unreachable();
}

#include "modules/wrappers.h"
#include "modules/io.h"
// #include "modules/init.h"

void init(void)
{
    // 0xffffff8000000000 - 0xffffff8000200000 [pt_ffffff8000000]    
    kprintnf("Greetz! - v0.1\n");

    halt();
}
