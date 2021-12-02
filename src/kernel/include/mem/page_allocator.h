#ifndef __ARCUS_INCLUDE_PAGE_ALLOCATOR
#define __ARCUS_INCLUDE_PAGE_ALLOCATOR
#include"mmap.h"
#include"init/init.h"

namespace arcus::memory
{
    void add_page_to_buddy(struct page* pg);
    void free_page(struct page* pg);
}
#endif
