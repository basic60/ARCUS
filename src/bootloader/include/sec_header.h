#ifndef __INCLUDE_ARCUS_BOOTLOADER_ELF_SECTION_HEADER
#define __INCLUDE_ARCUS_BOOTLOADER_ELF_SECTION_HEADER
#include"types.h"
#define EI_NIDENT 16
struct elf_shdr {
    uint32 sh_name;
    uint32 sh_type;
    uint64 sh_flags;
    uint64 sh_addr;     // 节的虚拟地址
    uint64 sh_offset;   // 如果该节存在于文件中，则表示该节在文件中的偏移；否则无意义，如sh_offset对于BSS 节来说是没有意义的
    uint64 sh_size;     // 节大小
    uint32 link;
    uint32 sh_info;
    uint64 sh_addralign;
    uint64 sh_entsize;
} ;
#endif