CORES = $(shell grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu)
DISK = kernel.img
FSC_OBJ = ./bootloader/boot.o
LOADER_OBJ = ./bootloader/loader.o
FORMATOR = ../tools/mkmyfs/mkmyfs
KERNEL = ./kernel/arcus_kernel

all: build qemu

build: clean build_bootloader build_kernel
ifneq ($(FORMATOR), $(wildcard $(FORMATOR)))
	$(MAKE) all -j$(CORES) -C ../tools/mkmyfs
endif
ifneq ($(DISK), $(wildcard $(DISK)))
	dd if=/dev/zero of=$(DISK) bs=1024 count=524288
	sleep 5
endif
	$(FORMATOR) $(DISK) -f
	dd if=$(FSC_OBJ) of=$(DISK) conv=notrunc
	dd if=$(LOADER_OBJ) of=$(DISK) conv=notrunc seek=2
	$(FORMATOR) $(DISK) -w $(KERNEL)
	sleep 1

.PHONY: build_bootloader
build_bootloader:
	$(MAKE) all -j$(CORES) -C bootloader

.PHONY: build_kernel
build_kernel:
	$(MAKE) all -j$(CORES) -C kernel

.PHONY: qemu
qemu:
	qemu-system-x86_64 -hda $(DISK)

.PHONY: clean
clean:
	$(MAKE) clean -C bootloader
	$(MAKE) clean -C kernel
