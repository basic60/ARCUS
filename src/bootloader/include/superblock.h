#ifndef __INCLUDE_ARCUS_BOOTLOADER_SPBLOCK
#define __INCLUDE_ARCUS_BOOTLOADER_SPBLOCK
#define ARFS_SIG 0xAF01

struct superblock {
    unsigned long long inode_count;
    unsigned long long block_count;
    unsigned long long free_inode_count;
    unsigned long long free_block_count;
    unsigned long long block_group_count;
    unsigned int data_blocks_per_group;
    unsigned int inodes_per_group;
    unsigned int checksum;
    unsigned short signature;
    char padding[4042];
};

#endif