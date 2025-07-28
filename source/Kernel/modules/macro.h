#ifndef MACRO_H
#define MACRO_H

#define BTEXT   __attribute__((section(".text.boot")))
#define TEXT    __attribute__((section(".text")))

#define ALIGNED __attribute__((aligned(1)))

#define GDT64_CODE_PTR 0x08

#endif
