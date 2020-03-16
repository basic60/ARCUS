#ifndef __INCLUDE_ARCUS_BOOTLOADER_GPDESC
#define __INCLUDE_ARCUS_BOOTLOADER_GPDESC
#include"types.h"
struct block_group_desc {
    uint64 inode_bitmap_lba;
    uint64 data_bitmap_lba;
    uint64 inode_table_lba;
    uint64 data_block_lba;
    uint32 free_inode_count;
    uint32 free_data_block_count;
    uint16 checksum;
    
    uint8 padding[22];
};
#endif