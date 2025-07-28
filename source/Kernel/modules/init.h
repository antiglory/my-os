#ifndef INIT_H
#define INIT_H

#include "init/map.h"
#include "init/idt.h"
#include "init/pic.h"
#include "init/pit.h"

void init(void)
{
    init_pic();
    init_pit();

    asm volatile("sti\n\t");

    jmp(&main);
}

#endif
