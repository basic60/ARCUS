#include"mem/slab.h"
#include"printk.h"

namespace arcus::memory
{
    // 用于分配kmem_cache结构
    static kmem_cache root_cache;
    static kmem_cache* kmalloc_cache_lst[10];

    static void init_slab_page_freelist(page* page, int obj_size, int align) {
        int real_size = align > obj_size 
            ? align 
            : (obj_size % align == 0 
                ? obj_size 
                : align * (obj_size / align + 1)
            );
        uint64 start_addr = page->virtual_address;
        uint64 limit = start_addr + (1 << EXTRACT_PAGE_ORDER(page) << PAGE_SHIFT);
        if (start_addr % real_size != 0) 
            start_addr += real_size - start_addr % real_size;
        
        page->obj = {0, 0};
        page->freelist = (void*)start_addr;
        /// 每一个未分配的对象指向下一个可用对象
        while (start_addr + real_size < limit) {
            *((uint64*) start_addr) = start_addr + real_size;
            start_addr += real_size;
            page->obj.tot++;
        }    
    }

    void* kmem_alloc(kmem_cache* slab) {
        // partial链表为空时从buddy中分配页
        if (slab->partial == nullptr) {
            slab->partial = alloc_pages(slab->order);
            slab->page_count += (1 << slab->order);
            slab->partial->slab = slab;
            slab->partial->lru = LIST_HEAD_INIT(slab->partial->lru);
            init_slab_page_freelist(slab->partial, slab->obj_size, slab->align);
        }

        page* pg = slab->partial;
        void* result = pg->freelist;
        pg->obj.inuse++;
        slab->obj_count++;

        if (pg->obj.tot == pg->obj.inuse) {
            // slab页中所有对象都被分配，则从partial链表中移除该页
            pg->freelist = nullptr;
            list_del(&slab->partial->lru);
        } else {
            pg->freelist = (void*)(*(uint64*) pg->freelist);
        }

        if (slab->ctor != nullptr)
            slab->ctor(result);
        return result;
    }

    void kmem_free(page* pg, void* obj) {
        kmem_cache* cache = pg->slab;
        cache->obj_count--;
        pg->obj.inuse--;
        // slab页中所有的对象都被释放，则释放该页
        if (!pg->obj.inuse) {
            pg->slab = nullptr;
            pg->freelist = nullptr;
            cache->page_count -= (1 << EXTRACT_PAGE_ORDER(pg));
            free_pages(pg);
            return;
        }
        // slab页从无可用到重新有可用对象可用时，重新加入partial链表
        if (pg->obj.inuse + 1 == pg->obj.tot) {
            list_add_after(&pg->lru, &cache->partial->lru);
        }

        uint64 nxt_obj = (uint64) pg->freelist;
        pg->freelist = obj;
        *(uint64*) pg->freelist = nxt_obj;
    }

    kmem_cache* kmem_create_slab(const char* slab_name, int obj_sz, int align, int order, void (*ctor)(void*)) {
        kmem_cache* cache = (kmem_cache*) kmem_alloc(&root_cache);
        *cache = {
            .slab_name = slab_name,
            .partial = nullptr,
            .obj_size = obj_sz,
            .align = align,
            .order = order,
            .obj_count = 0,
            .page_count = 0,
            .ctor = ctor
        };
        return cache;
    }

    static kmem_cache* choose_cache(int size) {
        if (size <=    8) return kmalloc_cache_lst[0];
        if (size <=   16) return kmalloc_cache_lst[1];
        if (size <=   32) return kmalloc_cache_lst[2];
        if (size <=   64) return kmalloc_cache_lst[3];
        if (size <=   96) return kmalloc_cache_lst[4];
        if (size <=  128) return kmalloc_cache_lst[5];
        if (size <=  256) return kmalloc_cache_lst[6];
        if (size <=  512) return kmalloc_cache_lst[7];
        if (size <= 1024) return kmalloc_cache_lst[8];
        if (size <= 2048) return kmalloc_cache_lst[9];
    }

    void* kmalloc(size_t sz) {
        if (sz > PAGE_SIZE >> 1) {
            int pg_cnt = (sz >> PAGE_SHIFT) + sz % PAGE_SIZE == 0 ? 0 : 1;
            int order = -1;
            while (1 << ++order < pg_cnt) ;
            return (void*) alloc_pages(order)->virtual_address;
        }

        kmem_cache* cache = choose_cache(sz);
        return kmem_alloc(cache);
    }

    void kfree(void* obj) {
        page* pg = PFN_TO_PAGE((uint64) obj >> PAGE_SHIFT);
        if (pg->slab == nullptr) {
            free_pages(pg);
            return;
        }
        kmem_cache* cache = pg->slab;
        kmem_free(pg, obj);
    }

    void init_kmem_cache() {
        root_cache = {
            .slab_name = "root_cache",
            .partial = nullptr,
            .obj_size = sizeof(struct kmem_cache),
            .align = 1,
            .order = BOOT_SLAB_PAGE_ORDER,
            .obj_count = 0,
            .page_count = 0,
            .ctor = nullptr
        };

        kmalloc_cache_lst[0] = kmem_create_slab("kmalloc-8", 8, 1, 1, nullptr);
        kmalloc_cache_lst[1] = kmem_create_slab("kmalloc-16", 16, 1, 1, nullptr);
        kmalloc_cache_lst[2] = kmem_create_slab("kmalloc-32", 32, 1, 1, nullptr);
        kmalloc_cache_lst[3] = kmem_create_slab("kmalloc-64", 64, 1, 1, nullptr);
        kmalloc_cache_lst[4] = kmem_create_slab("kmalloc-96", 96, 1, 1, nullptr);
        kmalloc_cache_lst[5] = kmem_create_slab("kmalloc-128", 128, 1, 1, nullptr);
        kmalloc_cache_lst[6] = kmem_create_slab("kmalloc-256", 256, 1, 1, nullptr);
        kmalloc_cache_lst[7] = kmem_create_slab("kmalloc-512", 512, 1, 1, nullptr);
        kmalloc_cache_lst[8] = kmem_create_slab("kmalloc-1024", 1024, 1, 1, nullptr);
        kmalloc_cache_lst[9] = kmem_create_slab("kmalloc-2048", 2048, 1, 1, nullptr);
    }
}

