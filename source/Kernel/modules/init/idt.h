#ifndef IDT_H
#define IDT_H

__attribute__((noreturn)) static void gp_isr(void)
{
    kprintf("#GP got caught\n");

    panic();
}

__attribute__((noreturn)) static void pf_isr(void)
{
    kprintf("#PF got caught\n");

    panic();
}

__attribute__((noreturn)) static void df_isr(void)
{
    kprintf("#DF got caught\n");

    panic();
}

#define eoi_out()                   \
    asm volatile                    \
    (                               \
        "movb $0x20, %%al\n\t"      \
        "outb %%al, $0x20\n\t"      \
        : : : "al"                  \
    )                               \

static struct irq_frame
{
    uint64_t rip;
    uint64_t cs;
    uint64_t flags;
    uint64_t rsp;
    uint64_t ss;
};

__attribute__((interrupt)) static void irq0_isr(struct irq_frame* instance)
{
    cpu_ticks++;
    eoi_out();
}

__attribute__((interrupt)) static void irq1_isr(struct irq_frame* instance)
{
    void write_back(const char c)
    {
        if (stdin_listen == false) return;
        else
        {
            size_t len = 0;

            while (len < STDIN_BUFF_SIZE && stdin_buffer[len] != '\0')
            {
                len++;
            }

            if (len + 1 < STDIN_BUFF_SIZE)
            {
                if (c == '\0')
                {
                    stdin_buffer[len] = '\n';

                    kprintf("\n");
                }
                else
                {
                    stdin_buffer[len] = c;
                    stdin_buffer[len + 1] = '\0';

                    kprintf("%c", c);
                }
            }
        }
    }

    uint8_t al;

    asm volatile
    (
        "inb $0x60, %0\n\t"
        : "=a"(al)
        :
    );

    if (al & 0x80) // ignore key release
    {
        eoi_out();

        return;
    }
    else if (al == 0x1C) // return (enter key)
    {
        write_back('\0');
        goto _end;
    }

    static const char ascii[] =
    {
        0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
        'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
        'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' '
    };

    if (al < sizeof(ascii))
    {
        char c = ascii[al];

        if (c != 0 && c >= ' ' && c <= '~')
            write_back(c);
    }
_end:
    eoi_out();
}

static struct idt_entry
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

void init_idt(void)
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

    // faults
    idt_set_gate(8,  (uintptr_t)df_isr,   GDT64_CODE_PTR, 0x8E);
    idt_set_gate(13, (uintptr_t)gp_isr,   GDT64_CODE_PTR, 0x8E);
    idt_set_gate(14, (uintptr_t)pf_isr,   GDT64_CODE_PTR, 0x8E);

    // IRQs
    idt_set_gate(32, (uintptr_t)irq0_isr, GDT64_CODE_PTR, 0x8E);
    idt_set_gate(33, (uintptr_t)irq1_isr, GDT64_CODE_PTR, 0x8E);

    asm volatile
    (
        "lidt %0\n\t"
        :
        : "m"(idtp)
    );

    kprintf("[ BOOT ] loaded IDT\n");

    return;
}

#endif
