ASM = nasm
CC = x86_64-elf-gcc
OBJCOPY = x86_64-elf-objcopy
LD = x86_64-elf-ld

FST_SECTOR_SRC = boot.asm
LOADER_C_SRC = $(shell find . -name "*.c")
LOADER_C_OBJ = $(patsubst %.c,%.o,$(LOADER_C_SRC))
LOADER_ASM_SRC = $(shell find . -name "*.asm" ! -path "./boot.asm")
LOADER_ASM_OBJ = $(patsubst %.asm,%.o,$(LOADER_ASM_SRC))

TARGET_FST_SECTOR = boot.o
TARGET_LOADER_TMP = loader_tmp.o
TARGET_LOADER = loader.o

all: clean first_sector kernel_loader

.PHONY: first_sector
first_sector: $(FST_SECTOR_SRC)
	$(ASM) $(FST_SECTOR_SRC) -f bin -g -o $(TARGET_FST_SECTOR)

.PHONY: kernel_loader
kernel_loader: link
	$(OBJCOPY) -O binary $(TARGET_LOADER_TMP) $(TARGET_LOADER)

.PHONY: link
link: $(LOADER_C_OBJ) $(LOADER_ASM_OBJ)
	$(LD) -nostdlib -Ttext 0x8000 $(LOADER_C_OBJ) $(LOADER_ASM_OBJ) -T scripts/loader.ld -o $(TARGET_LOADER_TMP)

%.o:%.c
	$(CC) -c -ffreestanding -I ./include -o $@ $<

%.o:%.asm
	$(ASM) -f elf64 -o $@ $<

.PHONY: clean
clean:
	-rm $(TARGET_FST_SECTOR) $(TARGET_LOADER_TMP) $(TARGET_LOADER) $(LOADER_C_OBJ) $(LOADER_ASM_OBJ)