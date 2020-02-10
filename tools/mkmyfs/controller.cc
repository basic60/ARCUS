#include"controller.h"
#include<string>
#include<iostream>
#include<sys/time.h>
#include<vector>
#include"dic_entry.h"
#include"inode.h"
using namespace std;
using namespace artools;

int controller::format(long long img_size) {
    init_superblock();
    fill_block_group_desc(img_size);
    mk_root_dir();
    return 0;
}

int controller::mk_root_dir() {
    long long inum = allocate_inode();
    cout<<"Root indoe is: "<<inum<<endl;
    long long dnum = allocate_data_block();
    inode* iptr = get_indoe_ptr(inum);
    new(iptr)inode(ROOT_UID, ROOT_GID, USER_EXEC | USER_READ | USER_WRITE | GROUP_READ | GROUP_EXEC | OTHER_READ | OTHER_EXEC, DIRECTORY);
    dic_entry tmp_dir(".", inum);
    add_dir_entry(iptr, &tmp_dir);
    return 0;
}

void controller::fill_block_group_desc(long long img_size) {
    int desc_tot_bytes = 0;
    long long cur = 0;
    int block_group_num = 0;
    while (cur < img_size - SUPER_BLOCK_OFFSET - sizeof(superblock)) {
        cur += BLOCK_GROUP_SIZE;

        block_group_num++;
        if (block_group_num % (BLOCK_SIZE / sizeof(block_group_desc)) == 1) {
            cur += BLOCK_SIZE;
        }

        if (cur > img_size - SUPER_BLOCK_OFFSET - sizeof(superblock)) {
            cur -= BLOCK_GROUP_SIZE;
            block_group_num--;
            break;
        }
    }

    int n = BLOCK_SIZE / sizeof(block_group_desc);
    long long block_group_offset = SUPER_BLOCK_OFFSET + sizeof(superblock) + 
        ((block_group_num + n - 1) / n)  * BLOCK_SIZE;
    this->spblock_ptr->block_group_count = block_group_num;

    block_desc_ptr = (block_group_desc*)(mm_addr + SUPER_BLOCK_OFFSET + sizeof(superblock));
    for (int i = 0; i < block_group_num; i++) {
        block_desc_ptr[i].free_inode_count = INODE_PER_GROUP;
        block_desc_ptr[i].free_data_block_count = DATA_BLOCKS_PER_GROUP;
        
        block_desc_ptr[i].inode_bitmap_lba = getlba(block_group_offset);
        ((block_bitmap*)get_ptr_by_lba(block_desc_ptr[i].inode_bitmap_lba))->clear();

        block_desc_ptr[i].data_bitmap_lba = getlba(block_group_offset + BLOCK_SIZE);
        ((block_bitmap*)get_ptr_by_lba(block_desc_ptr[i].data_bitmap_lba))->clear();

        block_desc_ptr[i].inode_table_lba = getlba(block_group_offset + BLOCK_SIZE * 2);
        block_desc_ptr[i].data_block_lba = getlba(block_desc_ptr[i].inode_table_lba + INODE_PER_GROUP * sizeof(inode));
        block_group_offset += BLOCK_GROUP_SIZE;

        spblock_ptr->free_inode_count += block_desc_ptr[i].free_inode_count;
        spblock_ptr->free_block_count += block_desc_ptr[i].free_data_block_count;
        spblock_ptr->inode_count += INODE_PER_GROUP;
        spblock_ptr->block_count += DATA_BLOCKS_PER_GROUP;
    }
}

void controller::init_superblock() {
    spblock_ptr->signature = ARFS_SIG;
    spblock_ptr->inodes_per_group = INODE_PER_GROUP;
    spblock_ptr->data_blocks_per_group = DATA_BLOCKS_PER_GROUP;
    spblock_ptr->inode_count = spblock_ptr->block_count 
        = spblock_ptr->free_block_count = spblock_ptr->free_inode_count = 0;
}

long long controller::getlba(long long offset) {
    return offset / SECTOR_SIZE;
}

void controller::print_spblock() {
    if (spblock_ptr->signature != ARFS_SIG) {
        cout<<"Unknown signature: "<<spblock_ptr->signature<<endl;
        return;
    }

    cout<<"Inode count:"<<spblock_ptr->inode_count<<endl;
    cout<<"Block count:"<<spblock_ptr->block_count<<endl;
    cout<<"Free Inode Count:"<<spblock_ptr->free_inode_count<<endl;
    cout<<"Free Block Count:"<<spblock_ptr->free_block_count<<endl;
    cout<<"Block Group Count:"<<spblock_ptr->block_group_count<<endl;
    cout<<"Data blocks per group"<<spblock_ptr->data_blocks_per_group<<endl;
    cout<<"Inodes per group:"<<spblock_ptr->inodes_per_group<<endl;
    return;
}

void controller::ls_root() {
    inode* ind = get_indoe_ptr(1);
    if (ind->file_type != DIRECTORY) {
        return;
    }
    cout<<endl<<"root dir indoe static:"<<endl;
    cout<<"uid: "<<ind->uid<<"  gid: "<<ind->gid<<endl;
    cout<<"bytes: "<<ind->bytes<<" tot:"<<ind->tot_size<<endl;

    for (int j = 0; j < 12 ;j++) {
        for (int i = 0; i < 8; i++) {
            dic_entry& dic = ((dic_entry*)get_data_block_ptr(ind->pt_direct[j]))[i];
            if (dic.inode_num != 0) {
                cout<<dic.name<<" indoe:"<<dic.inode_num<<endl;
            }
        }   
    }
}

void controller::write_file(const char* fname, uint8* buff, uint64 len) {
    uint64 finode = allocate_inode();
    inode* find_ptr = get_indoe_ptr(finode);
    new(find_ptr)inode(ROOT_UID, ROOT_GID, USER_EXEC | USER_READ | USER_WRITE | GROUP_READ | GROUP_EXEC | OTHER_READ | OTHER_EXEC, DIRECTORY);
    
    inode* dic_ind = get_indoe_ptr(1);
    vector<dic_entry*> des = list_dic_entry(dic_ind);
    for(auto i : des) {
        if (!strcmp(i->name, fname)) {
            cerr<<"same file!"<<endl;
            return;
        }
    }

    dic_entry dic_ent(fname, finode);
    add_dir_entry(dic_ind, &dic_ent);

    int dsize = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (int i = 0; i < dsize; i++) {
        append_data_block(find_ptr, allocate_data_block());
    }

    uint64 idx = 0;
    for(int i = 0; i < dsize; i++) {
        uint8* data = (uint8*)get_file_data_ptr(find_ptr, i);
        int j = 0;
        while (idx < len && j < BLOCK_SIZE) {
            data[j++] = buff[idx];
            idx++;
        }
    }
}


inode* controller::get_indoe_ptr(long long inum) {
    int desc_idx = (inum - 1) / INODE_PER_GROUP;
    int offset = (inum - 1) % INODE_PER_GROUP;
    long long itb_lba = this->block_desc_ptr[desc_idx].inode_table_lba;
    return (inode*)get_ptr_by_lba(itb_lba) + offset;
}

data_block* controller::get_data_block_ptr(long long dnum) {
    int desc_idx = (dnum - 1) / DATA_BLOCKS_PER_GROUP;
    int offset = (dnum - 1) % DATA_BLOCKS_PER_GROUP;
    long long d_lba = this->block_desc_ptr[desc_idx].data_block_lba;
    return (data_block*)get_ptr_by_lba(d_lba) + offset;
}

void* controller::get_ptr_by_lba(long long lba) {
    return (void*)(this->mm_addr + lba * SECTOR_SIZE);
}

controller::controller(uint8* addr) {
    this->mm_addr = addr;
    spblock_ptr = (superblock*)(this->mm_addr + SUPER_BLOCK_OFFSET);
    block_desc_ptr = (block_group_desc*)(this->mm_addr + SUPER_BLOCK_OFFSET + sizeof(superblock));
}

long long controller::allocate_inode() {
    for (int i = 0; i < this->spblock_ptr->block_group_count; i++) {
        block_group_desc& desc = this->block_desc_ptr[i];
        if (desc.free_inode_count > 0) {
            desc.free_inode_count--;
            this->spblock_ptr->free_inode_count--;
            block_bitmap* ibit = (block_bitmap*)get_ptr_by_lba(desc.inode_bitmap_lba);
            return i * INODE_PER_GROUP + ibit->poll() + 1;
        }
    }
    return 0;
}

long long controller::allocate_data_block() {
    for (int i = 0; i < this->spblock_ptr->block_group_count; i++) {
        block_group_desc& desc = this->block_desc_ptr[i];
        if (desc.free_data_block_count > 0) { 
            desc.free_data_block_count--;
            this->spblock_ptr->free_block_count--;
            block_bitmap* dbit = (block_bitmap*)get_ptr_by_lba(desc.data_bitmap_lba);
            uint64 dnum = i * DATA_BLOCKS_PER_GROUP + dbit->poll() + 1;
            get_data_block_ptr(dnum)->clear();
            return dnum;
        }
    }
    return 0; 
}

void controller::add_dir_entry(inode* di, dic_entry* dentry) {
    if (di->file_type != DIRECTORY) {
        return;
    }
 
    dic_entry* target_ptr;
    // Allocate a new data block
    if (di->bytes % BLOCK_SIZE == 0) {
        uint64 dnum = allocate_data_block();
        append_data_block(di, dnum);
        target_ptr = (dic_entry*)get_data_block_ptr(dnum);
    } else {
        uint64 didx = di->bytes / BLOCK_SIZE;
        uint64 doffset = di->bytes % BLOCK_SIZE;
        target_ptr = (dic_entry*)((uint8*)get_file_data_ptr(di, didx) + doffset);
    }

    memcpy(target_ptr, dentry, sizeof(dic_entry));
    di->bytes += sizeof(dic_entry);
}

void controller::write_data(inode* fi, uint8* buf, uint64 len) {
    if (fi->file_type != REGUALR_FILE) {
        return;
    }


}

void* controller::get_file_data_ptr(inode* ind, uint64 didx) {
    if (didx < 12) {
        return get_data_block_ptr(ind->pt_direct[didx]);
    } else if (didx < 12 + 512) {
        return get_data_block_ptr(((uint64*)get_data_block_ptr(ind->pt1))[didx - 12]);
    } else if (didx < 12 + 512 + 512 * 512) {
        uint64 idx1 = ((uint64*)get_data_block_ptr(didx - 12 - 512))[(didx - 12 - 512) / 512];
        return get_data_block_ptr(((uint64*)get_data_block_ptr(idx1))[didx - 12 - 512 - idx1 * 512]);
    } else if (didx < 12 + 512 + 512 * 512 * 512) {
        uint64 idx1 = ((uint64*)get_data_block_ptr(didx - 12 - 512 * 512))[(didx - 12 - 512 - 512 * 512) / (512 * 512)];
        uint64 idx2 = ((uint64*)get_data_block_ptr(idx1))[(didx - 12 - 512 * 512 - idx1 * 512 * 512) / 512];
        return get_data_block_ptr(((uint64*)get_data_block_ptr(idx2))[didx - 12 - 512 * 512 - idx1 * 512 * 512 - idx2 * 512]);
    }
    return nullptr;
}

int controller::append_data_block(inode* id, uint64 dnum) {
    if ((id->tot_size + BLOCK_SIZE) / BLOCK_SIZE > 12 + 512 + 512 * 512 + 512 * 512 * 512) {
        cerr<<"This file is too large!"<<endl;
        return -1;
    }

    int bcount =  id->tot_size / BLOCK_SIZE;
    if (bcount < 12) {
        id->pt_direct[bcount] = dnum;
    } else if (bcount < 12 + 512) {
        if (id->pt1 == 0) {
            id->pt1 = allocate_data_block();
        }

        uint64* val = (uint64*)get_data_block_ptr(id->pt1);
        val[bcount - 12] = dnum;
    } else if (bcount < 12 + 512 + 512 * 512) {
        if (id->pt2 == 0) {
            id->pt2 = allocate_data_block();
        }
        uint64* ptr1 = (uint64*)get_data_block_ptr(id->pt2);

        int idx1 = (bcount - 12 - 512) / 512;
        if (ptr1[idx1] == 0) {
            ptr1[idx1] = allocate_data_block();
        }

        uint64* ptr2 = (uint64*)get_data_block_ptr(ptr1[idx1]);
        ptr2[bcount - 12 - 512 - idx1 * 512] = dnum;
    } else if (bcount < 12 + 512 + 512 * 512 + 512L * 512 * 512) {
        if (id->pt3 == 0) {
            id->pt3 = allocate_data_block();
        }

        uint64* ptr1 = (uint64*)get_data_block_ptr(id->pt3);

        int idx1 = (bcount - 12 - 512 - 512 * 512) / (512 * 512);

        if (ptr1[idx1] == 0) {
            ptr1[idx1] = allocate_data_block();
        }

        uint64* ptr2 = (uint64*)get_data_block_ptr(ptr1[idx1]);
        int idx2 = (bcount - 12 - 512 - 512 * 512 - idx1 * 512 * 512) / 512;
        if (ptr2[idx2] == 0) {
            ptr2[idx2] = allocate_data_block();
        }

        uint64* ptr3 = (uint64*)get_data_block_ptr(ptr2[idx2]);
        ptr3[bcount - 12 - 512 - 512 * 512 - idx1 * 512 * 512 - idx2 * 512] = dnum;
    }
    id->tot_size += BLOCK_SIZE;
    return 0;
}

vector<dic_entry*> controller::list_dic_entry(inode* ind) {
    vector<dic_entry*> ret;
    if (ind->file_type != DIRECTORY) {
        return ret;
    }

    bool finish = false;
    for(int i = 0; i < 12; i++) {
        if (ind->pt_direct[i] == 0) {
            return ret;
        }
        dic_entry* dptr = (dic_entry*)get_data_block_ptr(ind->pt_direct[i]);
        for(int j = 0; j < BLOCK_SIZE / sizeof(dic_entry); j++) {
            if (!dptr[j].inode_num) {
                return ret;
            }
            ret.push_back(&dptr[j]);
        }

        uint64* p1 = (uint64*)get_data_block_ptr(ind->pt1);
        for(int i = 0; i < BLOCK_SIZE / sizeof(uint64); i++) {
            if (p1[i] == 0) {
                return ret;
            }

            dic_entry* dptr = (dic_entry*)get_data_block_ptr(p1[i]);
            for(int j = 0; j < BLOCK_SIZE / sizeof(dic_entry); j++) {
                if (!dptr[j].inode_num) {
                    return ret;
                }
                ret.push_back(&dptr[j]);
            }
        }

        p1 = (uint64*)get_data_block_ptr(ind->pt2);
        for(int i = 0; i < BLOCK_SIZE / sizeof(uint64); i++) {
            if (p1[i] == 0) {
                return ret;
            }
            for(int j = 0; j < BLOCK_SIZE / sizeof(uint64); j++) {
                uint64* p2 = (uint64*)get_data_block_ptr(p1[i]);
                if (p2[j] == 0) {
                    return ret;
                }

                dic_entry* dptr = (dic_entry*)get_data_block_ptr(p2[j]);
                for(int k = 0; k < BLOCK_SIZE / sizeof(dic_entry); k++) {
                    if (!dptr[k].inode_num) {
                        return ret;
                    }
                    ret.push_back(&dptr[k]);
                }

            }
        }

        p1 = (uint64*)get_data_block_ptr(ind->pt3);
        for(int i = 0; i < BLOCK_SIZE / sizeof(uint64); i++) {
            if (p1[i] == 0) {
                return ret;
            }
            for(int j = 0; j < BLOCK_SIZE / sizeof(uint64); j++) {
                uint64* p2 = (uint64*)get_data_block_ptr(p1[i]);
                if (p2[j] == 0) {
                    return ret;
                }

                for(int k = 0; k < BLOCK_SIZE / sizeof(uint64); k++) {
                    uint64* p3 = (uint64*)get_data_block_ptr(p2[j]);
                    if (p3[k] == 0) {
                        return ret;
                    }

                    dic_entry* dptr = (dic_entry*)get_data_block_ptr(p3[k]);
                    for(int l = 0; l < BLOCK_SIZE / sizeof(dic_entry); l++) {
                        if (!dptr[l].inode_num) {
                            return ret;
                        }
                        ret.push_back(&dptr[l]);
                    }
                }
            }
        }

    }
}