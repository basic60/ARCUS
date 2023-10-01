#ifndef __INCLUDE_ARCUE_INTERRUPT_TIMER
#define __INCLUDE_ARCUE_INTERRUPT_TIMER
#include"types.h"
#include"init/init.h"
namespace arcus::interrupt
{
    #define __INTERRUPT_TIMER_FREQUENCY 200
    void __init init_timer(uint32 frequency);

    uint64 get_runing_ms();

}
#endif