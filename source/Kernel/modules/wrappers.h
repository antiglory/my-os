#ifndef WRAPPERS_H
#define WRAPPERS_H

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