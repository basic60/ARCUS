#include"init/init.h"
#include"mem/mem_block.h"
#include"mem/mmap.h"
#include"mem/page_allocator.h"
#include"mem/slab.h"
#include"interrupt/idt.h"
#include"interrupt/timer.h"
#include"task/task.h"
#include"cpu/ap.h"
#include"printk.h"

namespace arcus
{
    // 初始化内核
    void init_kernel() {
        clear_text_screnn();
        // 初始化memblock,确保在伙伴算法等完善的内存分配算法未初始化完成之前支持简单的内存分配
        memory::init_memblock();
        // 初始化伙伴算法页面分配器
        memory::init_buddy();
        // 初始化内存映射，映射完整内存
        memory::init_mem_page();
        // 初始化slab分配器和内存分配函数，至此内核内存分配相关功能初始化完成
        memory::init_kmem_cache();
        // 初始化中断处理程序
        interrupt::init_interrupt();
        task::init_task();
        // 初始化计时器
        interrupt::init_timer(200); // 5ms一次        
        asm __volatile__("sti");
        // 多核CPU初始化
        cpu::startup_ap();
    }
}