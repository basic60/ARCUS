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

    enum e820_type {
        E820_TYPE_RAM	    = 1,
        E820_TYPE_RESERVED	= 2,
        E820_TYPE_ACPI		= 3,
        E820_TYPE_NVS		= 4,
        E820_TYPE_UNUSABLE	= 5,
        E820_TYPE_PMEM		= 7,
    };

    // 初始化memblock，用于启动阶段，内存分配算法尚未完成初始化之前分配内存临时使用。
    int init_memblock() __init;

    struct mem_region {
        uint64 base;
        uint64 size;
    };

    void* mblock_allocate(uint64 len, uint64 aligned = 0) __init;

    uint64 getMemoryBound();
}
#endif
