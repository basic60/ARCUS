#ifndef __INCLUDE_ARCUS_BOOTLOADER_ELF
#define __INCLUDE_ARCUS_BOOTLOADER_ELF
#include"types.h"
#define EI_NIDENT 16
struct elf_head {
    unsigned char e_ident[EI_NIDENT];
    uint16 e_type;
    uint16 e_machine;
    uint32 e_version;
    uint64 e_entry;
    uint64 e_phoff;
    uint64 e_shoff;
    uint32 e_flags;
    uint16 e_ehsize;
    uint16 e_phentsize;
    uint16 e_phnum;
    uint16 e_shentsize;
    uint16 e_shnum;
    uint16 e_shstrndx;
};
#endif