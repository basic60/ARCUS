#include"mem/mem_block.h"

namespace arcus::memory
{
    #define MAX_BUDDY_EXPONENT 10

    class BuddyPageAllocator {
    private:
        static int x[MAX_BUDDY_EXPONENT];
    public:
        static void init(arcus::memory::mem_region* x);
        static void* kalloc_pages();
    };
}