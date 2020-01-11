ASM = nasm

BOOT_LOADER_SRC = boot.asm
TARGET = boot.o

all:
	@echo [Compile] Compiling BootLoader
	$(ASM) $(BOOT_LOADER_SRC) -f bin -g -o $(TARGET)  

.PHONY: clean
clean:
	-rm $(TARGET)