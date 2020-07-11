#include"mem/mmap.h"
#include"types.h" 
#include"mem/mem_block.h"
#include"printk.h"
#include"str.h"
namespace arcus::memory
{
    static uint64 p1e[PAGE_ENTRY_CNT] __attribute__ ((aligned(PAGE_SIZE)));
    static int base;

    extern "C" void change_page_table(uint64* addr);
    static void mem_first_1gb();


    void init_mem_page() {
        mem_first_1gb();
    }

    void mem_first_1gb() {
        base = 0;
        uint64* p2e_0 = (uint64*)mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
        memset(p2e_0, 0, PAGE_ENTRY_CNT * sizeof(uint64));

        p1e[0] = (uint64)p2e_0 | PAGE_PRESENT | PAGE_RW;
        uint64* p3e_0 = (uint64*)mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
        memset(p3e_0, 0, PAGE_ENTRY_CNT * sizeof(uint64));
        p2e_0[0] = (uint64)p3e_0 | PAGE_PRESENT | PAGE_RW;
        for (int i = 0; i < PAGE_ENTRY_CNT; i++) {
            uint64* p4e = (uint64*)mblock_allocate(PAGE_ENTRY_CNT * sizeof(uint64), PAGE_ALIGN);
            p3e_0[i] = (uint64)p4e | PAGE_PRESENT | PAGE_RW;

            for(int j = 0; j < PAGE_ENTRY_CNT; j++) {
                p4e[j] = base | PAGE_PRESENT | PAGE_RW;
                base += PAGE_SIZE;
            }
        }

        change_page_table(p1e);
    }
}