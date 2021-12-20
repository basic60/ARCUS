#ifndef __ARCUS_INCLUDE_MBLOCK
#define __ARCUS_INCLUDE_MBLOCK
#include"types.h"
#include"init/init.h"

namespace arcus::memory
{
    #define MAX_BUDDY_SIZE 10
    #define E820_BASE 0xc004
    #define E820_CNT_PTR 0xc000
    #define INIT_MEM_CEILING 0x1a00000 // 26MB
    #define MAX_E820_ENTRY 128

    struct e820_entry {
        uint64 base;
        uint64 limit;
        uint32 type;
        uint32 acpi;
    };

    struct mem_range {
        uint64 base;
        uint64 limit;
    };

    enum e820_type {
        E820_TYPE_RAM	    = 1,
        E820_TYPE_RESERVED	= 2,
        E820_TYPE_ACPI		= 3,
        E820_TYPE_NVS		= 4,
        E820_TYPE_UNUSABLE	= 5,
        E820_TYPE_PMEM		= 7,
    };

    // 初始化memblock，用于启动阶段，内存分配算法尚未完成初始化之前分配内存临时使用。
    void init_memblock() __init;

    void* mblock_allocate(uint64 len, int aligned = 0) __init;
    mem_range alloc_all_over_memory(uint64 start_addr, int aligned) __init;
    e820_entry* get_free_entries() __init;
    uint64 get_max_memory_addr();

}
#endif
