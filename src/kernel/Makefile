CC = x86_64-elf-g++
ASM_SRC=$(shell find . -name "*.asm" ! -path "./crt/*")
ASM_OBJ=$(patsubst %.asm,%.o,$(ASM_SRC))
CPP_SRC = $(shell find . -name "*.cc")
CPP_OBJ = $(patsubst %.cc,%.o,$(CPP_SRC))
CRT_BEG_OBJ=$(shell x86_64-elf-g++ -print-file-name=crtbegin.o)
CRT_END_OBJ=$(shell x86_64-elf-g++ -print-file-name=crtend.o)
TARGET=arcus_kernel

all: link

.PHONY: link
link: $(CPP_OBJ) $(ASM_OBJ) ./crt/crti.o ./crt/crtn.o
	$(CC) -nostdlib -T ./scripts/kernel.ld ./crt/crti.o $(CRT_BEG_OBJ) $(CPP_OBJ) $(ASM_OBJ) $(CRT_END_OBJ) ./crt/crtn.o -lgcc -o $(TARGET)

%.o:%.cc
	$(CC) -std=c++17 -g -c -ffreestanding -fno-rtti -fno-exceptions -fno-use-cxa-atexit -I ./include -o $@ $<

%.o:%.asm
	nasm -f elf64 -o $@ $<

clean:
	-rm $(TARGET) $(ASM_OBJ) $(CPP_OBJ) ./crt/crti.o ./crt/crtn.o

