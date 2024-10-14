void _start() __attribute__((section(".text.boot"))) __attribute__((naked));

void halt()
{
    asm volatile (
        "hlt \n\t"
    );

    return; // ?
}

// kernel print not formatted
void kprintnf(unsigned char* str) {
    unsigned short* vmem = (unsigned short*)0xB8000;
    unsigned short ref = 0x1F00;

    for (char i = 0; str[i] != '\0'; i++) {
        vmem[i] = (ref | str[i]);
    }
}

void _start()
{
    kprintnf("a l i v e");

    halt();
}
