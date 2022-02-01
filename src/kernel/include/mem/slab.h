#ifndef __ARCUS_INCLUDE_SLAB
#define __ARCUS_INCLUDE_SLAB
#include"page_allocator.h"
namespace arcus::memory
{
    #define BOOT_SLAB_PAGE_ORDER 1
    #define KMALLOC_BLOC_NUM 10
    #define MAX_NODE 

    struct kmem_cache {
        const char* slab_name;  // 仅展示使用 
        struct page* partial;
        int obj_size;
        int align;
        int order;
        uint64 obj_count;       // 已分配的总对象个数
        uint64 page_count;      // slab包含的总页数
        void (*ctor)(void*);
    };

    kmem_cache* kmem_create_slab(const char* slab_name, int size, int align, int order, void (*ctor)(void*));

    void* kmem_alloc(kmem_cache* slab);

    void kmem_free(void* obj);

    void* kmalloc(size_t size);

    void kfree(void* obj);

    void init_kmem_cache() __init;

}
#endif