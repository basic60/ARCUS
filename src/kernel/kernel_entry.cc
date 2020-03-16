#include "ktypes.h"
using namespace arcus;
extern "C" void kernel_entry();

static uint16* video_memory_char=(uint16*)(0xB8000);
void kernel_entry() {
    uint8 attrByte=0x03;
    uint16 blank='a' | (attrByte<<8);  // 32 is the ascii of blank.
    uint16 i;
    for(i=0;i<80*25;i++){
		  video_memory_char[i]=blank;
    }
}