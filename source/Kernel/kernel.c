void _start() __attribute__((section(".text.boot"))) __attribute__((naked));

void _start()
{
    asm volatile (
        "mov $0xB8000, %rax \n\t"
        "movw $0x1F4B, (%rax) \n\t"
    );

    while (1);
}
