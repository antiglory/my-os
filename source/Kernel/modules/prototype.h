#ifndef PROTOTYPE_H
#define PROTOTYPE_H

// init.h
TEXT  ALIGNED void init(void);
TEXT  ALIGNED void init_map_kernel_sections(void);
TEXT  ALIGNED void init_idt(void);

// self
BTEXT ALIGNED void kstart(uint64_t* pml4, uint64_t* pdpt, uint64_t* pd, uint64_t* pt)  __attribute__((naked));
TEXT  ALIGNED void main(void);

// ../../Linker/kernel.ld
extern char __stack_start[];
extern char __stack_end[];
extern uintptr_t _kernel_vma;
extern uintptr_t _kernel_lma;
uint64_t* pt_tables;
uint64_t* pd_table;
uint64_t* pdpt_table;
uint64_t* pml4_table;

#endif
