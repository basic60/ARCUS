#ifndef __ARCUS_INCLUDE_MMAP
#define __ARCUS_INCLUDE_MMAP
#include"init/init.h"
#include"types.h"
namespace arcus::memory
{
    struct page {
        uint64 virtual_address;
        int type;
        struct list_head lru;
    };

    #define PAGE_ENTRY_CNT 512
    #define PAGE_ALIGN 4096
    #define PAGE_SIZE 4096
    #define PAGE_SHIFT 12
    #define PAGE_RW 2
    #define PAGE_PRESENT 1
    // 所有struct page结构映射基地址
    #define VMEME_MAP_BASE 0x100000000000

    #define vmemmap ((struct page*) VMEME_MAP_BASE)

    #define PFN_TO_PAGE(pfn) (vmemmap + (pfn))
    #define PAGE_TO_PFN(page) (unsigned long long)((page) - vmemmap)

    void init_mem_page() __init;
    // 将物理地址映射到虚拟地址
    void map_virt_to_phy(uint64 phy_addr, uint64 virt_addr);

}
#endif
