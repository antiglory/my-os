#ifndef WRAPPERS_H
#define WRAPPERS_H

TEXT  ALIGNED void halt(void)    __attribute__((noreturn));

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

#endif
