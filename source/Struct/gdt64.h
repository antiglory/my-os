#ifndef GDT64_H
#define GDT64_H

// GDT for 64 bits (long mode)
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed, aligned(8)));

void def64gdt(struct gdt_entry* gdt)
{
    gdt[0] = (struct gdt_entry){0}; // null descriptor

    gdt[1] = (struct gdt_entry){
        .limit_low   = 0xFFFF,
        .base_low    = 0,
        .base_middle = 0,
        .access      = 0x9A,      // code segment
        .granularity = 0xA0, // 64-bit mode
        .base_high   = 0
    };

    gdt[2] = (struct gdt_entry){
        .limit_low   = 0xFFFF,
        .base_low    = 0,
        .base_middle = 0,
        .access      = 0x92,      // data segment
        .granularity = 0xC0, // 64-bit mode
        .base_high   = 0
    };
}

#endif