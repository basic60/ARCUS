#include"mem/mem_block.h"
#include"printk.h"

extern int __KERNEL_END__; // 内核结束地址为   &__KERNEL_END__ 

namespace arcus::memory
{
    struct memory_end {
        e820_entry* ent_ptr;

        uint64 addr;
    };

    static e820_entry entries[MAX_E820_ENTRY] __initdata;
    static e820_entry new_entries[MAX_E820_ENTRY] __initdata;
    static int sanitized_entries_cnt __initdata;
    static e820_entry* overlap_list[MAX_E820_ENTRY] __initdata;
    static memory_end end_points[MAX_E820_ENTRY * 2] __initdata;
    static mem_block mblock __initdata;
    static mem_region free_region[MAX_E820_ENTRY] __initdata;
    static mem_region reserved_region[MAX_E820_ENTRY] __initdata;
    

    static void sort_end_points(memory_end* ptr, int cnt) __init;
    static void sanitize_e820_entries() __init;
 
    int init_memblock() {
        sanitize_e820_entries();

        mblock.free.cnt = mblock.reserved.cnt = 0;
        mblock.free.max = mblock.reserved.max = MAX_E820_ENTRY;
        mblock.free.reg = free_region;
        mblock.reserved.reg = reserved_region;
        uint64 kend = (uint64)(&__KERNEL_END__);
        // 内存地址低于kend直接标记为不可用。
        for (int i = 0; i < sanitized_entries_cnt; i++) {
            if (new_entries[i].type == E820_TYPE_RAM) {
                if (new_entries[i].base + new_entries[i].limit <= kend) {
                    mblock.reserved.add_region(new_entries[i].base, new_entries[i].limit);
                } else if (new_entries[i].base < kend && new_entries[i].base + new_entries[i].limit > kend) {
                    mblock.reserved.add_region(new_entries[i].base, kend - new_entries[i].base);
                    mblock.free.add_region(kend, new_entries[i].base + new_entries[i].limit - kend);
                } else {
                    mblock.free.add_region(new_entries[i].base, new_entries[i].limit);
                }
            } else {
                mblock.reserved.add_region(new_entries[i].base, new_entries[i].limit);
            }
        }
    }

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

    static void sort_end_points(memory_end* ptr, int cnt) {
        for (int i = 0; i < cnt - 1; i++) {
            for (int j = i + 1; j < cnt; j++) {
                if (ptr[i].addr > ptr[j].addr 
                    || ptr[i].addr == ptr[j].addr
                    && ptr[i].ent_ptr->base + ptr[i].ent_ptr->limit 
                        > ptr[j].ent_ptr->base + ptr[j].ent_ptr->limit) {
                    memory_end tmp_ent;
                    tmp_ent = ptr[i];
                    ptr[i] = ptr[j];
                    ptr[j] = tmp_ent;
                }
            }
        }
    }

    void mem_type::add_region(uint64 base, uint64 size) {
        if (this->cnt >= this->max - 5) {
            this->max *= 2;
            mem_region* new_array = (mem_region*)mblock_allocate(sizeof(struct mem_region) * this->max);
            for (int i = 0 ;i < cnt; i++) {
                new_array[i] = this->reg[i];
            }

            if (this->max / 2 > MAX_E820_ENTRY) mblock_free(this->reg);
            
            this->reg = new_array;
        }
        this->reg[this->cnt].base = base;
        this->reg[this->cnt++].size = size;
    }

    void mem_type::remove_region(int rmcnt) {
        if (rmcnt > this->max || rmcnt >= cnt) {
            return;
        }        
        this->reg[rmcnt] = this->reg[this->cnt - 1];
        this->cnt--;
    }

    void mem_type::merge_region() {
        for (int i = 0; i < this->cnt; i++) {
            for (int j = i + 1; j < this->cnt; j++) {
                if (this->reg[i].base + this->reg[i].size == this->reg[j].base) {
                    this->reg[i].size += this->reg[j].size;
                    this->remove_region(j);
                } else if (this->reg[j].base + this->reg[j].size == this->reg[i].base) {
                    this->reg[j].size += this->reg[i].size;
                    this->remove_region(i);
                }
            }
        }
    }

    void* mblock_allocate(uint64 len, uint64 aligned) {
        uint64 ret = 0;
        uint64 padding = 0;
        uint64 kend = (uint64)&__KERNEL_END__;
        for (int i = 0 ; i < mblock.free.cnt; i++) {
            if (aligned > 0 && mblock.free.reg[i].base % aligned != 0) {
                len += (padding = aligned - mblock.free.reg[i].base % aligned);
            }

            if (mblock.free.reg[i].size >= len && mblock.free.reg[i].base + len <= INIT_MEM_CEILING) {
                mblock.reserved.add_region(mblock.free.reg[i].base, len);
                ret = mblock.free.reg[i].base + padding;
                if (len < mblock.free.reg[i].size) {
                    mblock.free.add_region(mblock.free.reg[i].base + len, mblock.free.reg[i].size - len);
                }
                mblock.free.remove_region(i);
                return (void*)ret;
            }
        }
        return nullptr;
    }

    void mblock_free(void* addr) {
        for (int i = 0; i < mblock.reserved.cnt; i++) {
            if (mblock.reserved.reg[i].base <= (uint64) addr && mblock.reserved.reg[i].base + mblock.reserved.reg[i].size > (uint64) addr) {
                mblock.free.add_region(mblock.reserved.reg[i].base, 
                mblock.reserved.reg[i].size);
                mblock.free.merge_region();
                mblock.reserved.remove_region(i);
                break;
            }
        }
    }
}
