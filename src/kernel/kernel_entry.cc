#include"types.h"
#include"init/init.h"
#include"printk.h"
using namespace arcus;
extern "C" void kernel_entry();

static uint16* video_memory_char=(uint16*)(0xB8000);
void kernel_entry() {
    init_kernel();
}