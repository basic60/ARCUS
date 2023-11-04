#ifndef __ARCUS_INCLUDE_CPU_AP
#define __ARCUS_INCLUDE_CPU_AP
#include"types.h"
#include"init/init.h"

namespace arcus::cpu
{
    #define __MAX_CPU 8
    // 获取当前cpuid
    extern "C" uint32 get_apic_id();

    // 启动多核cpu
    void __init startup_ap();

}
#endif