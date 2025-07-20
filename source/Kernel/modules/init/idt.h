#ifndef IDT_H
#define IDT_H

/*
void interrupt_handler0(void)
{
    kprintf("trigged ISR 0\n");
}

void interrupt_handler1(void)
{
    kprintf("trigged ISR 1\n");
}
*/

static void gp_isr(void)
{
    kprintf("#GP got caught\n");

    halt();
}

static void pf_isr(void)
{
    kprintf("#PF got caught\n");

    halt();
}

/*
static void isr0(void)
{
    asm volatile
    (
        "push %%rax\n\t"
        "push %%rcx\n\t"
        "push %%rdx\n\t"
        "push %%rbx\n\t"
        "push %%rbp\n\t"
        "push %%rsi\n\t"
        "push %%rdi\n\t"
        
        "call interrupt_handler0\n\t"
        
        "pop %%rdi\n\t"
        "pop %%rsi\n\t"
        "pop %%rbp\n\t"
        "pop %%rbx\n\t"
        "pop %%rdx\n\t"
        "pop %%rcx\n\t"
        "pop %%rax\n\t"

        "iretq"
        :
        :
        : "memory"
    );
}

static void isr1(void)
{
    asm volatile
    (
        "push %%rax\n\t"
        "push %%rcx\n\t"
        "push %%rdx\n\t"
        "push %%rbx\n\t"
        "push %%rbp\n\t"
        "push %%rsi\n\t"
        "push %%rdi\n\t"
        
        "call interrupt_handler1\n\t"

        "pop %%rdi\n\t"
        "pop %%rsi\n\t"
        "pop %%rbp\n\t"
        "pop %%rbx\n\t"
        "pop %%rdx\n\t"
        "pop %%rcx\n\t"
        "pop %%rax\n\t"

        "iretq"
        :
        :
        : "memory"
    );
}
*/

struct idt_entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  flags;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr   idtp;

static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_mid  = (base >> 16) & 0xFFFF;
    idt[num].base_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector  = sel;
    idt[num].ist       = 0x0;
    idt[num].flags     = flags;
    idt[num].reserved  = 0x0;
}

void idt_init(void)
{
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint64_t)&idt;

    // clean
    for (int32_t i = 0; i < 256; i++)
    {
        idt_set_gate(i, 0x0, 0x0, 0x0);
    }

    // 0x08 = GDT64 code selector
    // 0x8E - present | ring 0 | interrupt gate
    idt_set_gate(13, (uint64_t)gp_isr, 0x08, 0x8E);
    idt_set_gate(14, (uint64_t)pf_isr, 0x08, 0x8E);

    asm volatile
    (
        "lidt %0"
        :
        : "m"(idtp)
    );
}

#endif
