#include"mem/mmap.h"
#include"types.h" 
#include"mem/mem_block.h"
#include"str.h"
namespace arcus::memory
{
    static uint64 p1e[PAGE_ENTRY_CNT] __attribute__ ((aligned(PAGE_SIZE)));
    static uint64 base __attribute__((section(".data"))) = 0;

    extern "C" void change_page_table(uint64* addr);


    void init_mem_page() {
        uint64* p2e_0 = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
        memset(p2e_0, 0, PAGE_ENTRY_CNT * sizeof(uint64));

        p1e[0] = (uint64)p2e_0 | PAGE_PRESENT | PAGE_RW;
        uint64* p3e_0 = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
        memset(p3e_0, 0, PAGE_ENTRY_CNT * sizeof(uint64));
        p2e_0[0] = (uint64)p3e_0 | PAGE_PRESENT | PAGE_RW;
        for (int i = 0; i < PAGE_ENTRY_CNT; i++) {
            uint64* p4e = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
            p3e_0[i] = (uint64) p4e | PAGE_PRESENT | PAGE_RW;

            for(int j = 0; j < PAGE_ENTRY_CNT; j++) {
                p4e[j] = base | PAGE_PRESENT | PAGE_RW;
                base += PAGE_SIZE;
            }
        }
        // 先映射1GB，防止内存不组无法建立完成的页表映射。
        change_page_table(p1e);

        uint64 max_gb = (getMemoryBound() -1) / (1024 * 1024 * 1024) + 1;
        for (int i = 1; i < max_gb; i++) {
            uint64* p3e_i = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
            memset(p3e_i, 0, PAGE_ENTRY_CNT * sizeof(uint64));
            p2e_0[i] = (uint64) p3e_i | PAGE_PRESENT | PAGE_RW;
            for (int j = 0; j < PAGE_ENTRY_CNT; j++) {
                uint64* p4e = (uint64*) mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
                p3e_i[j] = (uint64) p4e | PAGE_PRESENT | PAGE_RW;

                for(int k = 0; k < PAGE_ENTRY_CNT; k++) {
                    p4e[k] = base | PAGE_PRESENT | PAGE_RW;
                    base += PAGE_SIZE;
                }
            }
        }
        // 映射完整的页表
        change_page_table(p1e);
    }
}