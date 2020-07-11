#ifndef __ARCUS_INCLUDE_MMAP
#define __ARCUS_INCLUDE_MMAP
#include"init/init.h"
namespace arcus::memory
{
    #define PAGE_ENTRY_CNT 512
    #define PAGE_ALIGN 4096
    #define PAGE_SIZE 4096
    #define PAGE_RW 2
    #define PAGE_PRESENT 1
    void init_mem_page() __init;
}
#endif
