#ifndef __ARCUS_INCLUDE_PAGE_ALLOCATOR
#define __ARCUS_INCLUDE_PAGE_ALLOCATOR
#include"mmap.h"
#include"init/init.h"

namespace arcus::memory
{
    #define MAX_PAGE_ORDER_CNT 11
    #define EXTRACT_PAGE_ORDER(pg) ((uint64) pg->type ^ ((uint64) pg->type >> 4 << 4))
    #define SET_PAGE_ORDER(pg, order) (pg->type = (pg->type >> 4 << 4) | order)

    struct buddy_free_area {
        size_t nr_free;
        struct list_head* free_lst;
    };

    void init_buddy() __init;
    struct page* alloc_pages(int order);
    int free_pages(struct page* pg);
}
#endif
