#ifndef __ARCUS_TOOLS_DIR_H
#define __ARCUS_TOOLS_DIR_H
#include"types.h"
namespace artools
{
    struct dic_entry {
        char name[256];
        uint64 inode_num;
        uint32 name_len;
        uint8 padding[244];

        dic_entry();
        dic_entry(const char* name, uint64 inode_num);
    };
}
#endif