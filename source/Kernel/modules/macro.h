#ifndef MACRO_H
#define MACRO_H

#define BTEXT   __attribute__((section(".text.boot")))
#define TEXT    __attribute__((section(".text")))
#define ALIGNED __attribute__((aligned(1)))

#define JMP(addr)                   \
    asm volatile                    \
    (                               \
        "jmp *%0\n\t"               \
        :                           \
        : "r" ((void*)addr)         \
        :                           \
    )

#endif