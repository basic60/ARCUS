#ifndef __ARCUS_TOOLS_DATA_BLOCK_H
#define __ARCUS_TOOLS_DATA_BLOCK_H
#include"types.h"
#include"block_bitmap.h"
#include"inode.h"
namespace artools
{
    struct data_block {
        uint8 data[4096];
        
        void clear();
    };
}
#endif