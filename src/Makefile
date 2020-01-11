CORES = $(shell grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu)
DISK = kernel.img
IPL_OBJ = ./bootloader/boot.o

all: build qemu

build: clean build_bootloader
ifneq ($(DISK), $(wildcard $(DISK)))
	dd if=/dev/zero of=kernel.img bs=1024 count=14400
	sleep 3
endif
	dd if=$(IPL_OBJ) of=kernel.img conv=notrunc

.PHONY: build_bootloader
build_bootloader:
	$(MAKE) all -j$(CORES) -C bootloader

.PHONY: qemu
qemu:
	qemu-system-x86_64 -hda kernel.img

.PHONY: clean
clean:
	$(MAKE) clean -C bootloader
