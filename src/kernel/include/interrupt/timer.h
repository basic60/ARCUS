#ifndef __INCLUDE_ARCUE_INTERRUPT_TIMER
#define __INCLUDE_ARCUE_INTERRUPT_TIMER
#include"types.h"
#include"init/init.h"
namespace arcus::interrupt
{
    void __init init_timer(uint32 frequency);
}
#endif