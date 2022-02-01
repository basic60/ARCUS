#include"mem/mmap.h"
#include"types.h" 
#include"mem/mem_block.h"
#include"str.h"
#include"printk.h"
#include"mem/page_allocator.h"

extern long long __KERNEL_END__;
namespace arcus::memory
{
    static uint64 p1e[PAGE_ENTRY_CNT] __attribute__ ((aligned(PAGE_SIZE)));
    static uint64 base __attribute__((section(".data"))) = 0;
    static uint64 cur_vmemmap_addr __attribute__((section(".data"))) = 0;

    extern "C" void change_page_table(uint64* addr);

    void populate_vmemmap_range(uint64 phy_addr_base, uint64 limit) {
        if (limit < PAGE_SIZE) return;
        uint64 cur_base = phy_addr_base;
        struct page* cur_page = (struct page*) VMEME_MAP_BASE;

        int tt = 1;
        do {
            map_virt_to_phy_early(cur_base, cur_base);
            struct page* cur_page = PFN_TO_PAGE(cur_base >> PAGE_SHIFT);
            // 判断是否需要申请新的一页内存放置struct page结构
            if ((uint64) cur_page + sizeof(page) - cur_vmemmap_addr > PAGE_SIZE) {
                cur_vmemmap_addr = ((uint64) cur_page + sizeof(page)) >> PAGE_SHIFT << PAGE_SHIFT;
                map_virt_to_phy_early(cur_vmemmap_addr, (uint64) mblock_allocate(PAGE_SIZE, PAGE_ALIGN));
            }
            cur_page->virtual_address = cur_base;
            cur_page->type = 0;
            cur_page->obj = {0, 0};
            cur_page->freelist = nullptr;
            cur_page->slab = nullptr;
            free_pages(cur_page);

            cur_base += PAGE_SIZE;
            limit -= PAGE_SIZE;
        } while(limit >= PAGE_SIZE);
    }

    void init_sparse_vmemmap() {
        mem_range free_mem_range;
        // 将512MB以上所有内存加入vmalloc区域
        while ((free_mem_range = alloc_all_over_memory(0x20000000, PAGE_SIZE)).base != 0) {
            populate_vmemmap_range(free_mem_range.base, free_mem_range.limit);
        }
    }

    void init_mem_page() {
        // 0 ~ 512MB内存采用一一映射(identity map)
        memset(p1e, 0, PAGE_ENTRY_CNT * sizeof(uint64));
        for (uint64 i = 0; i < 0x20000000; i+= PAGE_SIZE) {
            map_virt_to_phy_early(i, i);
        }
        change_page_table(p1e);
        // 使用sparse_vmemmap映射剩余的内存
        init_sparse_vmemmap();
    }

    // 将虚拟地址映射到指定的物理地址
    void map_virt_to_phy_early(uint64 virt_addr, uint64 phy_addr) {
        int p1e_idx = virt_addr >> 39 & 0x1ff;
        int p2e_idx = virt_addr >> 30 & 0x1ff;
        int p3e_idx = virt_addr >> 21 & 0x1ff;
        int p4e_idx = virt_addr >> 12 & 0x1ff;

        if (p1e[p1e_idx] == 0) {
            uint64* p2e = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
            uint64* p3e = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
            uint64* p4e = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
            memset(p2e, 0, PAGE_ENTRY_CNT * sizeof(uint64));
            memset(p3e, 0, PAGE_ENTRY_CNT * sizeof(uint64));
            memset(p4e, 0, PAGE_ENTRY_CNT * sizeof(uint64));
            p1e[p1e_idx] = (uint64) p2e | PAGE_PRESENT | PAGE_RW;
            p2e[p2e_idx] = (uint64) p3e | PAGE_PRESENT | PAGE_RW;
            p3e[p3e_idx] = (uint64) p4e | PAGE_PRESENT | PAGE_RW;
            p4e[p4e_idx] = phy_addr | PAGE_PRESENT | PAGE_RW;
            return;
        }
        
        uint64* p2e = (uint64*) ((p1e[p1e_idx] >> PAGE_SHIFT) << PAGE_SHIFT);
        if (p2e[p2e_idx] == 0) {
            uint64* p3e = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
            uint64* p4e = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
            memset(p3e, 0, PAGE_ENTRY_CNT * sizeof(uint64));
            memset(p4e, 0, PAGE_ENTRY_CNT * sizeof(uint64));
            p2e[p2e_idx] = (uint64) p3e | PAGE_PRESENT | PAGE_RW;
            p3e[p3e_idx] = (uint64) p4e | PAGE_PRESENT | PAGE_RW;
            p4e[p4e_idx] = phy_addr | PAGE_PRESENT | PAGE_RW;
            return;
        }

        uint64* p3e = (uint64*) ((p2e[p2e_idx] >> PAGE_SHIFT) << PAGE_SHIFT);
        if (p3e[p3e_idx] == 0) {
            uint64* p4e = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
            memset(p4e, 0, PAGE_ENTRY_CNT * sizeof(uint64));
            p3e[p3e_idx] = (uint64) p4e | PAGE_PRESENT | PAGE_RW;
            p4e[p4e_idx] = phy_addr | PAGE_PRESENT | PAGE_RW;
            return;
        }

        uint64* p4e = (uint64*) ((p3e[p3e_idx] >> PAGE_SHIFT) << PAGE_SHIFT);
        p4e[p4e_idx] = phy_addr | PAGE_PRESENT | PAGE_RW;
        return;
    }
}