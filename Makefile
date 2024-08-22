# tools
NASM = nasm
DD = dd
LD = ld
CAT = cat
GCC = gcc
GPP = g++

# input
BOOTLOADER = source/Boot/bootloader.asm
PREKERNEL_IN = source/Kernel/prekernel.cpp

# output
BOOTLOADER_OUT = output/bootloader.bin
PREKERNEL_OBJ = output/prekernel.o
PREKERNEL_OUT = output/prekernel.bin
OS_IMAGE = output/os.img

# compile-flags
NASM_FLAGS = -f bin
PREKERNEL_FLAGS = -g -c -ffreestanding -fno-pie -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -m32 -O2 -fno-exceptions

# rule
all: $(OS_IMAGE)

# bootloader compilation
$(BOOTLOADER_OUT): $(BOOTLOADER)
	$(NASM) $(NASM_FLAGS) $< -o $@

# prekernel compilation
$(PREKERNEL_OBJ): $(PREKERNEL_IN)
	$(GPP) $(PREKERNEL_FLAGS) $< -o $@

# prekernel linking
$(PREKERNEL_OUT): $(PREKERNEL_OBJ)
	$(LD) -o $@ -Ttext 0x1000 $< --entry=main --oformat binary -m elf_i386

# OS image creation
$(OS_IMAGE): $(BOOTLOADER_OUT) $(PREKERNEL_OUT)
	$(DD) if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880
	$(DD) if=$(BOOTLOADER_OUT) of=$(OS_IMAGE) conv=notrunc
	$(DD) if=$(PREKERNEL_OUT) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc

clean:
	rm -f $(BOOTLOADER_OUT) $(PREKERNEL_OBJ) $(PREKERNEL_OUT) $(OS_IMAGE)

run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE)

dbg: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE) -s -S

rund: $(OS_IMAGE)
	qemu-system-x86_64 -D log.txt -drive format=raw,file=$(OS_IMAGE)

.PHONY: all clean run rund dbg
