# tools
NASM = nasm
DD = dd
LD = ld
CAT = cat
GCC = gcc

# input
BOOTLOADER = source/Boot/bootloader.asm
PREKERNEL = source/Kernel/prekernel.asm
KERNEL = source/Kernel/kernel.c

# output
BOOTLOADER_OUT = output/bootloader.bin
PREKERNEL_OBJ = output/prekernel.o
PREKERNEL_OUT = output/prekernel.bin
KERNEL_OBJ = output/kernel.o
KERNEL_OUT = output/kernel.bin
OS_IMAGE = output/os.img

# linker script
PK_LINKER = source/Linker/prekernel.ld
KERNEL_LINKER = source/Linker/kernel.ld

# compile-flags
NASM_FLAGS = -g -f elf32
NASM_BIN_FLAGS = -g -f bin
KERNEL_FLAGS = -g -c -ffreestanding -fno-pie -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -m64 -O2 -fno-exceptions -nostdlib -fno-omit-frame-pointer
KERNEL_LINK_FLAGS = -n -T $(KERNEL_LINKER) -o $(KERNEL_OUT) -nostdlib

# rule
all: $(OS_IMAGE)

# bootloader compilation
$(BOOTLOADER_OUT): $(BOOTLOADER)
	$(NASM) $(NASM_BIN_FLAGS) $< -o $@

# kernel compilation
$(KERNEL_OBJ): $(KERNEL)
	$(GCC) $(KERNEL_FLAGS) $< -o $@

# kernel linking
$(KERNEL_OUT): $(KERNEL_OBJ)
	$(LD) $(KERNEL_LINK_FLAGS) $^

# prekernel compilation
$(PREKERNEL_OBJ): $(PREKERNEL)
	$(NASM) $(NASM_FLAGS) $< -o $@

# prekernel linking
$(PREKERNEL_OUT): $(PREKERNEL_OBJ)
	$(LD) -o $@ -T $(PK_LINKER) $^ --entry=_pstart --oformat binary -m elf_i386

# OS image creation
$(OS_IMAGE): $(BOOTLOADER_OUT) $(PREKERNEL_OUT) $(KERNEL_OUT)
	$(DD) if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880
	$(DD) if=$(BOOTLOADER_OUT) of=$(OS_IMAGE) conv=notrunc
	$(DD) if=$(PREKERNEL_OUT) of=$(OS_IMAGE) bs=512 seek=1 count=7 conv=notrunc
	$(DD) if=$(KERNEL_OUT) of=$(OS_IMAGE) bs=512 seek=8 count=8 conv=notrunc

clean:
	rm -f $(BOOTLOADER_OUT) $(PREKERNEL_OBJ) $(PREKERNEL_OUT) $(KERNEL_OBJ) $(KERNEL_OUT) $(OS_IMAGE)

run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE)

dbg: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE) -s -S

log: $(OS_IMAGE)
	qemu-system-x86_64 -D log -drive format=raw,file=$(OS_IMAGE)

.PHONY: all clean run log dbg
