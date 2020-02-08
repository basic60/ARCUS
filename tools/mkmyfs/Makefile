CC = g++
CPP_SRC = $(shell find . -name "*.cc")
CPP_OBJ = $(patsubst %.cc,%.o,$(CPP_SRC))
TARGET = mkmyfs
DISK = test.img

all: clean $(CPP_OBJ) link
ifneq ($(DISK), $(wildcard $(DISK)))
	dd if=/dev/zero of=test.img bs=1024 count=524288
endif

%.o:%.cc
	@echo [Compile] Compiling C++ file
	$(CC) -std=c++17 -fno-strict-aliasing -g -c -m64 -I ./include -o $@ $<

link: $(CPP_OBJ)
	$(CC) $(CPP_OBJ) -o $(TARGET)

clean:
	-rm $(CPP_OBJ) $(TARGET)

