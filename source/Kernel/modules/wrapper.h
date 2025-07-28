#ifndef WRAPPERS_H
#define WRAPPERS_H

TEXT  ALIGNED void halt(void)    __attribute__((noreturn));

#define jmp(addr)                   \
    asm volatile                    \
    (                               \
        "jmp *%0\n\t"               \
        :                           \
        : "r" ((void*)addr)         \
        :                           \
    )                               \

void halt(void)
{
    while(1)
    {
        asm volatile
        (
            "cli\n\t"
            "hlt\n\t"
        );
    }
}

inline void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

inline void in(uint16_t port, uint8_t value)
{
    asm volatile ("in %0, %1" : : "a"(value), "Nd"(port));
}

#endif
