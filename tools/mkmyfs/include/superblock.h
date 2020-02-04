#ifndef __ARCUS_TOOLS_SUPERBLOCK_H
#define __ARCUS_TOOLS_SUPERBLOCK_H
#include "types.h"
namespace artools 
{
    #define ARFS_SIG 0xAF01

    struct superblock {
        uint64 inode_count;
        uint64 block_count;
        uint64 free_inode_count;
        uint64 free_block_count;
        uint64 block_group_count;
        uint32 data_blocks_per_group;
        uint32 inodes_per_group;
        uint32 checksum;
        uint16 signature;
        uint8 padding[4042];
    };
}

#endif
