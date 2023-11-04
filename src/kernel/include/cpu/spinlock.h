#ifndef __ARCUS_INCLUDE_SPINLOCK
#define __ARCUS_INCLUDE_SPINLOCK
#include"types.h"

namespace arcus::cpu
{
    struct spinlock {
        uint8 head;
        uint8 tail;
    };
    
    void rep_nop();

    void spin_lock(spinlock* lock);
    void spin_unlock(spinlock* lock);

    #define barrier() __asm__ __volatile__("": : :"memory")
}
#endif