#ifndef WRAPPERS_H
#define WRAPPERS_H

TEXT  ALIGNED void halt()    __attribute__((noreturn));

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
