#ifndef __ARCUS_TOOLS_CONTROLLER_H
#define __ARCUS_TOOLS_CONTROLLER_H
#include"gpdes.h"
#include"inode.h"
#include"superblock.h"
#include"block.h"
#include"data_block.h"
#include<memory>
#include<vector>
#include<string>
namespace artools
{
    #define SUPER_BLOCK_OFFSET 4096
    #define BLOCK_SIZE 4096
    #define DATA_BLOCKS_PER_GROUP 32768
    #define SECTOR_SIZE 512
    #define BLOCK_GROUP_SIZE (BLOCK_SIZE * 2 + INODE_PER_GROUP * 256 + DATA_BLOCKS_PER_GROUP * BLOCK_SIZE)
    class controller {
    private:
        superblock* spblock_ptr;
        block_group_desc* block_desc_ptr;
        superblock spblock;
        uint8* mm_addr;
    private:
        void init_superblock();
        void fill_block_group_desc(long long img_size);
 	    long long getlba(long long offset);
        
        /*********************************************************************************************************
         Get inode number, set bitmap and change remaining quantity in the superblock and group descriptors.
         Return 0 if failed.
        *********************************************************************************************************/   
        long long allocate_inode();

        /*********************************************************************************************************
         Get data block number, set bitmap and change remaining quantity in the superblock and group descriptors.
         Return 0 if failed.
        *********************************************************************************************************/
        long long allocate_data_block();

        /*********************************************************************************************************
         Get inode poiner through inode number.
        *********************************************************************************************************/
        inode* get_indoe_ptr(long long inum);

        /*********************************************************************************************************
         Get data block poiner through data block number.
        *********************************************************************************************************/
        data_block* get_data_block_ptr(long long dnum);

        /*********************************************************************************************************
         Get pointer through lba.
        *********************************************************************************************************/
        void* get_ptr_by_lba(long long lba);
        
        void set_inode_bitmap(long long inum, bool val);
        void set_data_bitmap(long long dnum, bool val);
    public:
        int format(long long size);
        int mk_root_dir();
        void print_spblock();
        controller(uint8* addr);
        controller() = delete;
        void write_into_kernel(uint8* kernel_addr, long long ksize);
    };
}
#endif
