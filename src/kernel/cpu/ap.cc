#include"cpu/ap.h"
#include"printk.h"

extern "C" void start_multi_core();

namespace arcus::cpu
{
    void startup_ap() {
        start_multi_core();   
    }

    void ttt() {
        printk("ok\n");
    }
}