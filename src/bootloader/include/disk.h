#ifndef __INCLUDE_ARCUS_BOOTLOADER_DISK
#define __INCLUDE_ARCUS_BOOTLOADER_DISK
#include"types.h"
extern void ld_read_disk(uint64 lba, uint64 count, uint64 target_addr);
#endif