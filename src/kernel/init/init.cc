#include"init/init.h"
#include"mem/mem_block.h"
#include"mem/mmap.h"
#include"printk.h"
namespace arcus
{
    // 初始化内核流程：
    // 1. 初始化memblock,确保在伙伴算法等完善的内存分配算法未初始化完成之前支持简单的内存分配。
    // 2. 映射全部内存地址。
    // 3. 初始化内存分配算法。
    void init_kernel() {
        clear_text_screnn();
        memory::init_memblock();
        memory::init_mem_page();
    }
}