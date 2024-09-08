#include <stdint.h>

#include "modules/io.cpp"
#include "../config.h"

extern "C" void LMS();

// bootloader -> carrega o prekernel em 0x1000 -> vai pro protegido -> jumpa pro prekernel -> prekernel jumpa pro label que carrega o modo longo -> retorna pro prekernel mas em op mode de modo longo -> vai pro kernel

extern "C" void __attribute__((section(".text.start"))) start()
{
    // state:
    // ESP and EBP are at 0x1000;
    // code/&main is running at 0x1000 and it is not gdt-based;
    // loaded from block 2 (hard disk);
    // 32 bits - protected mode - ring 0

    const char PK_ENTRYP_PK_REACHED[] = "[ INFO ] prekernel was reached\0";

    clear_screen();
    pkprintnf(PK_ENTRYP_PK_REACHED);

    LMS();

    while (1);
}
