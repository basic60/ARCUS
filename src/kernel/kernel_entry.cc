#include"types.h"
#include"init/init.h"
#include"printk.h"
using namespace arcus;
extern "C" void kernel_entry();

void kernel_entry() {
    init_kernel();
}