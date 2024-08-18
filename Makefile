# tools
NASM = nasm
DD = dd
CAT = cat
GCC = gcc
GPP = g++

# input
BOOTLOADER = source/Boot/bootloader.asm
PREKERNEL_IN = source/Kernel/prekernel.c
KERNEL_IN = source/Kernel/kernel.cpp

# output
BOOTLOADER_OUT = output/bootloader.bin
PREKERNEL_OUT = output/prekernel.bin
KERNEL_OUT = output/kernel.bin
OS_IMAGE = output/os.img

# compile-flags
NASM_FLAGS = -f bin
GCC_PREKERNEL_FLAGS = -o $(PREKERNEL_OUT) -static $(PREKERNEL_IN)
GPP_KERNEL_FLAGS = -o $(KERNEL_OUT) $(KERNEL_IN)

# rule
all: $(OS_IMAGE)

# bootloader compilation
$(BOOTLOADER_OUT): $(BOOTLOADER)
	$(NASM) $(NASM_FLAGS) $< -o $@

# prekernel compilation
$(PREKERNEL_OUT): $(PREKERNEL_IN)
	$(GCC) $(GCC_PREKERNEL_FLAGS)

# OS image linking
$(OS_IMAGE): $(BOOTLOADER_OUT) $(PREKERNEL_OUT)
	$(DD) if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880
	$(DD) if=$(BOOTLOADER_OUT) of=$(OS_IMAGE) conv=notrunc
	$(DD) if=$(PREKERNEL_OUT) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc

clean:
	rm -f $(BOOTLOADER_OUT) $(PREKERNEL_OUT) $(OS_IMAGE)

run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE)

dbg: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE) -s -S

rund: $(OS_IMAGE)
	qemu-system-x86_64 -D log.txt -drive format=raw,file=$(OS_IMAGE)

.PHONY: all clean run rund dbg
