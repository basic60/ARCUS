#include"mem/mem_block.h"
#include"printk.h"
#include"types.h"

extern long long __KERNEL_END__; // 内核结束地址为&__KERNEL_END__，定义于kernel.ld
extern long long __INIT_BEGIN__;
extern long long __INIT_END__;

namespace arcus::memory
{
    struct e820_end {
        e820_entry* ent_ptr;

        uint64 addr;
    };

    static e820_entry new_entries[MAX_E820_ENTRY] __initdata;
    static e820_entry free_entries[MAX_E820_ENTRY] __initdata;
    static int sanitized_entries_cnt __initdata;
    static int free_entries_cnt __initdata;
    static e820_entry* overlap_list[MAX_E820_ENTRY] __initdata;
    static e820_end end_points[MAX_E820_ENTRY * 2] __initdata;
    static uint64 max_memory_addr;

    static void sort_end_points(e820_end* ptr, int cnt) __init;
    static void sanitize_e820_entries() __init;
 
    void init_memblock() {
        sanitize_e820_entries();

        uint64 kend = (uint64)(&__KERNEL_END__);
        free_entries_cnt = 0;
        for (int i = 0; i < sanitized_entries_cnt; i++) {
            if (new_entries[i].type != E820_TYPE_RAM || new_entries[i].base + new_entries[i].limit < kend) continue;
            free_entries[free_entries_cnt].base = new_entries[i].base < kend ? kend : new_entries[i].base;
            free_entries[free_entries_cnt++].limit = new_entries[i].base < kend ? new_entries[i].limit - kend + new_entries[i].base : new_entries[i].limit;
        }
        max_memory_addr = free_entries[free_entries_cnt - 1].base + free_entries[free_entries_cnt - 1].limit;
    }

    // 整理e820表项
    static void sanitize_e820_entries() {
        int entry_cnt = *((int*)E820_CNT_PTR);
        e820_entry* e820_entry_ptr = (e820_entry*)E820_BASE;
        
        int end_point_cnt = 0;
        int overlap_cnt = 0;
        int current_type = 0;
        int last_type = 0;
        uint64 last_addr = 0;
        int new_entry_cnt = 0;
        for (int i = 0; i < entry_cnt; i++) {
            if (e820_entry_ptr[i].limit == 0) continue;
            
            end_points[end_point_cnt].ent_ptr = &e820_entry_ptr[i];
            end_points[end_point_cnt++].addr = e820_entry_ptr[i].base;

            end_points[end_point_cnt].ent_ptr = &e820_entry_ptr[i];
            end_points[end_point_cnt++].addr = e820_entry_ptr[i].base + e820_entry_ptr[i].limit;
            if (i + 1 >= MAX_E820_ENTRY) break;
        }
        sort_end_points(end_points, end_point_cnt);

        for (int i = 0; i < end_point_cnt; i++) {
            if (end_points[i].addr == end_points[i].ent_ptr->base) {
                // endpoint是内存段的起点则在overlap_list中新增段。
                overlap_list[overlap_cnt++] = end_points[i].ent_ptr;
            } else {
                for (int j = 0; j < overlap_cnt; j++) {
                    if (overlap_list[j] == end_points[i].ent_ptr) {
                        overlap_list[j] = overlap_list[overlap_cnt - 1];
                        break;
                    }
                }
                overlap_cnt--;
            }

            current_type = 0;
            for (int j = 0; j < overlap_cnt; j++) {
                if (overlap_list[j]->type > current_type) {
                    current_type = overlap_list[j]->type;
                }
            }
            if (current_type != last_type) {
                if (last_type != 0) {
                    new_entries[new_entry_cnt].limit = end_points[i].addr - last_addr;
                    if (new_entries[new_entry_cnt].limit > 0) {
                        if (++new_entry_cnt > MAX_E820_ENTRY) break;
                    }
                }
                if (current_type != 0) {
                    new_entries[new_entry_cnt].base = end_points[i].addr;
                    new_entries[new_entry_cnt].type = current_type;
                    last_addr = new_entries[new_entry_cnt].base;
                }
                last_type = current_type;                
            }
        }
        sanitized_entries_cnt = new_entry_cnt;
    }

    static void sort_end_points(e820_end* ptr, int cnt) {
        for (int i = 0; i < cnt - 1; i++) {
            for (int j = i + 1; j < cnt; j++) {
                if (ptr[i].addr > ptr[j].addr 
                    || ptr[i].addr == ptr[j].addr
                    && ptr[i].ent_ptr->base + ptr[i].ent_ptr->limit 
                        > ptr[j].ent_ptr->base + ptr[j].ent_ptr->limit) {
                    e820_end tmp_ent;
                    tmp_ent = ptr[i];
                    ptr[i] = ptr[j];
                    ptr[j] = tmp_ent;
                }
            }
        }
    }

    // 分配内存，内核初始化阶段临时使用。
    void* mblock_allocate(uint64 len, int aligned) {
        for (int i = 0; i < free_entries_cnt; i++) {
            int64 alloc_base = free_entries[i].base % aligned == 0 ? free_entries[i].base : free_entries[i].base + aligned - free_entries[i].base % aligned;
            if (alloc_base - free_entries[i].base + len > free_entries[i].limit) continue;
            free_entries[i].limit -= alloc_base - free_entries[i].base + len;
            free_entries[i].base = alloc_base + len;
            return (void*) alloc_base;
        }
        return nullptr;
    }

    // 分配大于开始地址的所有可用内存
    mem_range alloc_all_over_memory(uint64 start_addr, int aligned) {
        mem_range ragne = {0, 0};
        for (int i = 0; i < free_entries_cnt; i++) {
            uint64 alloc_base = start_addr + start_addr % aligned;
            if (free_entries[i].limit == 0 || free_entries[i].base + free_entries[i].limit <= alloc_base)
                continue;
            if (free_entries[i].base < alloc_base) {
                ragne.base = alloc_base;
                ragne.limit = free_entries[i].limit - alloc_base;
                free_entries[i].limit = alloc_base - free_entries[i].base;
            } else {
                ragne.base = free_entries[i].base;
                ragne.limit = free_entries[i].limit;
                free_entries[i].limit = 0;
            }
            return ragne;
        }
        return ragne;
    }

    uint64 get_max_memory_addr() {
        return max_memory_addr;
    }

}
