# tools
NASM = nasm
DD = dd
CAT = cat

# input
BOOTLOADER = source/Boot/bootloader.asm
PREKERNEL = source/Kernel/prekernel.asm

# output
BOOTLOADER_BIN = output/bootloader.bin
PREKERNEL_BIN = output/prekernel.bin
OS_IMAGE = output/os.img

# compile-flags
NASM_FLAGS = -f bin

# rule
all: $(OS_IMAGE)

# bootloader compilation
$(BOOTLOADER_BIN): $(BOOTLOADER)
	$(NASM) $(NASM_FLAGS) $< -o $@

# prekernel compilation
$(PREKERNEL_BIN): $(PREKERNEL)
	$(NASM) $(NASM_FLAGS) $< -o $@

# OS image linking
$(OS_IMAGE): $(BOOTLOADER_BIN) $(PREKERNEL_BIN)
	$(DD) if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880
	$(DD) if=$(BOOTLOADER_BIN) of=$(OS_IMAGE) conv=notrunc
	$(DD) if=$(PREKERNEL_BIN) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc

clean:
	rm -f $(BOOTLOADER_BIN) $(PREKERNEL_BIN) $(OS_IMAGE)

run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE)

runs: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE) -s -S

.PHONY: all clean run runs