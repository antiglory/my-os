void _kstart()  __attribute__((section(".text.boot"))) __attribute__((naked));
void halt()     __attribute__((noreturn));

extern char __stack_start[];
extern char __stack_end[];

void halt()
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

// kernel print not formatted (WIP)
void kprintnf(unsigned char* str)
{
    unsigned short* vmem = (unsigned short*)0xFFFFFF80000B8000; // canonical form of VGA mem
    unsigned short  ref  = 0x1F00;

    for (char i = 0; str[i] != '\0'; i++)
    {
        vmem[i] = (ref | str[i]);
    }
}

// kernel entrypoint
void _kstart()
{
    // setup stack
    asm volatile
    (
        "movq %0, %%rax\n\t"
        "movq %%rax, %%rsp\n\t"
        "movq %%rsp, %%rbp\n\t"
        "jmp main\n\t"
        : 
        : "r"(__stack_end)
        : "rax", "memory"
    );

    __builtin_unreachable();
}

void main()
{
    // 0xffffff8000000000 - 0xffffff8000200000 [pt_ffffff8000000]
    kprintnf("Greetz! - v0.1");

    halt();
}
