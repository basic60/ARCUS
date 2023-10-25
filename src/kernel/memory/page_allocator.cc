#include"mem/page_allocator.h"
#include"mem/mem_block.h"
#include"str.h"
#include"list.h"
#include"printk.h"

namespace arcus::memory
{
    static struct buddy_free_area free_areas[MAX_PAGE_ORDER_CNT];
    static int64* buddy_bitmap;

    static void chg_bitmap(int start_pfn, int order) {
        for (int i = 0; i < (1 << order); i++) {
            buddy_bitmap[(start_pfn + i) / 64] ^= 1LL << ((start_pfn + i) % 64);
        }
    }

    static bool buddy_free(int start_pfn, int order) {
        for (int i = 0; i < (1 << order); i++) {
            if(!(buddy_bitmap[(start_pfn + i) / 64] & (1LL << ((start_pfn + i) % 64)))) {
                return false;
            }
        }
        return true;
    }

    static void add_page_to_free_list(struct page* page, int order) {
        free_areas[order].nr_free++;
        if (free_areas[order].free_lst == nullptr) {
            free_areas[order].free_lst = &page->lru;
            page->lru = LIST_HEAD_INIT(page->lru);
        } else {
            list_add_after(&page->lru, free_areas[order].free_lst);
        }
    }

    static void remove_page_from_free_list(struct page* pg, int order) {
        free_areas[order].nr_free--;
        list_del(&pg->lru);
        if (free_areas[order].nr_free == 0)
            free_areas[order].free_lst = nullptr;
    }

    static void split_buddy_to_order(int order) {
        int now_order = order;
        struct page* cur_page = nullptr;
        while(++now_order < MAX_PAGE_ORDER_CNT) {
            if (!free_areas[now_order].nr_free) 
                continue;
            cur_page = LIST_ENTRY(free_areas[now_order].free_lst->next, page, lru);
            remove_page_from_free_list(LIST_ENTRY(free_areas[now_order].free_lst->next, page, lru), now_order);
            break;
        }
        if (cur_page == nullptr)
            return;

        struct page *lpage  = cur_page, *rpage;
        while(now_order > order) {
            now_order--;
            rpage = PFN_TO_PAGE(PAGE_TO_PFN(lpage) + (1 << now_order));
            SET_PAGE_ORDER(lpage, now_order);
            SET_PAGE_ORDER(rpage, now_order);
            add_page_to_free_list(rpage, now_order);
        }
        add_page_to_free_list(lpage, now_order);
    }

    /**
     * 分配2^order个连续的page
     */
    struct page* alloc_pages(int order) {
        if (order >= MAX_PAGE_ORDER_CNT)
            return nullptr;
        // 分裂大buddy块以支持非配需求
        if (!free_areas[order].nr_free)
            split_buddy_to_order(order);
        // 内存不足，无法支持分配
        if (!free_areas[order].nr_free)
            return nullptr;

        struct page* result = LIST_ENTRY(free_areas[order].free_lst->next, page, lru);
        remove_page_from_free_list(LIST_ENTRY(free_areas[order].free_lst->next, page, lru), order);
        chg_bitmap(PAGE_TO_PFN(result), order);
        return result;
    }

    /**
     * 释放page到buddy分配器
     */ 
    int free_pages(struct page* pg) {
        uint64 pfn = PAGE_TO_PFN(pg);
        int order = EXTRACT_PAGE_ORDER(pg);
        chg_bitmap(pfn, order);
        while(order + 1 < MAX_PAGE_ORDER_CNT) {
            uint64 buddy_pfn = pfn % (1 << (order + 1)) == 0 ? pfn + (1 << order) : pfn - (1 << order);
            // 无法合并成更大的buddy块
            if (!buddy_free(buddy_pfn, order)) 
                break;
            remove_page_from_free_list(PFN_TO_PAGE(buddy_pfn), order);
            pfn = pfn > buddy_pfn ? buddy_pfn : pfn;
            pg = PFN_TO_PAGE(pfn);
            order++;
        }
        SET_PAGE_ORDER(pg, order);
        add_page_to_free_list(pg, order);
        return 0;
    }

    void init_buddy() {
        uint64 bitmap_length = MAX_SUPPORT_MEM / 4096 / 8;
        buddy_bitmap = (int64*) mblock_allocate(bitmap_length, 1024);
        memset(buddy_bitmap, 0, bitmap_length);
        for (int i = 0; i < MAX_PAGE_ORDER_CNT; i++) {
            free_areas[i].nr_free = 0;
            free_areas[i].free_lst = nullptr;
        }
    }
}