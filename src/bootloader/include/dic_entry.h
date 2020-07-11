#ifndef __INCLUDE_ARCUS_BOOTLOADER_DIC_ENTRY
#define __INCLUDE_ARCUS_BOOTLOADER_DIC_ENTRY
#include"types.h"
struct dic_entry {
    char name[256];
    uint64 inode_num;
    uint32 name_len;
    uint8 padding[244];

};
#endif