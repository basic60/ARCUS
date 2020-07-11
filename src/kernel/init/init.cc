#include"init/init.h"
#include"mem/mem_block.h"
#include"mem/mmap.h"
namespace arcus
{
    void init_kernel() {
        memory::init_memblock();
        memory::init_mem_page();
    }
}