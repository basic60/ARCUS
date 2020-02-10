#ifndef __ARCUS_TOOLS_CONTROLLER_H
#define __ARCUS_TOOLS_CONTROLLER_H
#include"gpdes.h"
#include"inode.h"
#include"superblock.h"
#include"data_block.h"
#include"dic_entry.h"
#include<memory>
#include<vector>
#include<string>
namespace artools
{
    #define BLOCK_SIZE 4096
    #define SUPER_BLOCK_OFFSET 4096
    #define DATA_BLOCKS_PER_GROUP 32768
    #define SECTOR_SIZE 512    

    #define BLOCK_GROUP_SIZE (BLOCK_SIZE * 2 + INODE_PER_GROUP * 256 + DATA_BLOCKS_PER_GROUP * BLOCK_SIZE)
    #define ROOT_UID 1
    #define ROOT_GID 1
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

        /*********************************************************************************************************
         Add a directory entry.
        *********************************************************************************************************/
        void add_dir_entry(inode* ind, dic_entry* dentry);

        std::vector<dic_entry*> list_dic_entry(inode* ind);

        /*********************************************************************************************************
         Write data to inode.
        *********************************************************************************************************/
        void write_data(inode* ind, uint8* buf, uint64 len);

        /*********************************************************************************************************
         Append a datablock that inode pointes to. Return 0 if success else -1
        *********************************************************************************************************/
        int append_data_block(inode* ind, uint64 dnum);

        /*********************************************************************************************************
         Get data block ptr through offset.
        *********************************************************************************************************/
        void* get_file_data_ptr(inode* ind, uint64 block_idx);

        int mk_root_dir();
    public:
        int format(long long size);
        void print_spblock();
        void ls_root();
        controller(uint8* addr);
        controller() = delete;
        void write_file(const char* fname, uint8* buff, uint64 len);
    };
}
#endif
