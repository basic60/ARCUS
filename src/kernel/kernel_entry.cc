#include"types.h"
#include"init/init.h"
#include"printk.h"
#include"cpu/ap.h"
using namespace arcus;
extern "C" void kernel_entry();
static int bsp = -1;

void kernel_entry() {
    if (bsp == -1) {
        bsp = cpu::get_apic_id();
        init_kernel();
    }
}