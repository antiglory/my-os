# tools
NASM = nasm
DD = dd
LD = ld
CAT = cat
GCC = gcc
GPP = g++

PK_LINKER = source/Linker/prekernel.ld

# input
BOOTLOADER = source/Boot/bootloader.asm
PREKERNEL_IN = source/Kernel/prekernel.cpp
LMS_IN = source/Kernel/lms.asm

# output
BOOTLOADER_OUT = output/bootloader.bin
PREKERNEL_OBJ = output/prekernel.o
PREKERNEL_OUT = output/prekernel.bin
LMS_OBJ = output/lms.o
OS_IMAGE = output/os.img

# compile-flags
NASM_FLAGS = -g -f bin
PREKERNEL_FLAGS = -g -c -ffreestanding -fno-pie -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -m32 -O2 -fno-exceptions -nostdlib -fno-rtti

# rule
all: $(OS_IMAGE)

# bootloader compilation
$(BOOTLOADER_OUT): $(BOOTLOADER)
	$(NASM) $(NASM_FLAGS) $< -o $@

# LMS compilation
$(LMS_OBJ): $(LMS_IN)
	$(NASM) -f elf32 $< -o $@

# prekernel compilation
$(PREKERNEL_OBJ): $(PREKERNEL_IN)
	$(GPP) $(PREKERNEL_FLAGS) $< -o $@

# prekernel linking
$(PREKERNEL_OUT): $(PREKERNEL_OBJ) $(LMS_OBJ)
	$(LD) -o $@ -T $(PK_LINKER) $^ --entry=start --oformat binary -m elf_i386

# OS image creation
$(OS_IMAGE): $(BOOTLOADER_OUT) $(PREKERNEL_OUT)
	$(DD) if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880
	$(DD) if=$(BOOTLOADER_OUT) of=$(OS_IMAGE) conv=notrunc
	$(DD) if=$(PREKERNEL_OUT) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc

clean:
	rm -f $(BOOTLOADER_OUT) $(PREKERNEL_OBJ) $(PREKERNEL_OUT) $(LMS_OBJ) $(OS_IMAGE)

run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE)

dbg: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE) -s -S

rund: $(OS_IMAGE)
	qemu-system-x86_64 -D log.txt -drive format=raw,file=$(OS_IMAGE)

.PHONY: all clean run rund dbg
