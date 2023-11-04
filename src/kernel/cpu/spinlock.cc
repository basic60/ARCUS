#include"cpu/spinlock.h"
namespace arcus::cpu
{
    __attribute__ ((always_inline)) void rep_nop() {
        asm __volatile__("rep; nop":::"memory");
    }

    void spin_lock(spinlock* lock) {
        uint8 cur_tail = 1;
        asm __volatile__("xadd %0, %1"
            :"=r"(cur_tail),"=r"(lock->tail)
            :"0"(cur_tail),"1"(lock->tail)
            :"memory");

        for (;;) {
            if (lock->head == cur_tail) {
                goto out;
            }
            rep_nop();
        }
    out:
        barrier();
    }

    void spin_unlock(spinlock* lock) {
        lock->head++;
    }
}