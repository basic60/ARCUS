ASM = nasm
CC = x86_64-elf-gcc
OBJCOPY = x86_64-elf-objcopy
LD = x86_64-elf-ld

FST_PART_SRC = boot.asm
LOADER_C_SRC = $(shell find . -name "*.c")
LOADER_C_OBJ = $(patsubst %.c,%.o,$(LOADER_C_SRC))
LOADER_ASM_SRC = $(shell find . -name "*.asm" ! -path "./boot.asm")
LOADER_ASM_OBJ = $(patsubst %.asm,%.o,$(LOADER_ASM_SRC))

TARGET_FST_PART = boot.o
TARGET_LOADER_TMP = loader_tmp.o
TARGET_LOADER = loader.o

all: clean first_part kernel_loader

.PHONY: first_part
first_part: $(FST_PART_SRC)
	$(ASM) $(FST_PART_SRC) -f bin -g -o $(TARGET_FST_PART)

.PHONY: kernel_loader
kernel_loader: link
	$(OBJCOPY) -O binary $(TARGET_LOADER_TMP) $(TARGET_LOADER)

.PHONY: link
link: $(LOADER_C_OBJ) $(LOADER_ASM_OBJ)
	$(LD) -nostdlib -Ttext 0x8200 $(LOADER_C_OBJ) $(LOADER_ASM_OBJ) -T scripts/loader.ld -o $(TARGET_LOADER_TMP)

%.o:%.c
	$(CC) -c -w -ffreestanding -I ./include -o $@ $<

%.o:%.asm
	$(ASM) -f elf64 -o $@ $<

.PHONY: clean
clean:
	-rm $(TARGET_FST_PART) $(TARGET_LOADER_TMP) $(TARGET_LOADER) $(LOADER_C_OBJ) $(LOADER_ASM_OBJ)