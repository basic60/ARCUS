#include"print.h"
#include"types.h"
#include"superblock.h"
#include"disk.h"
#include"gpdes.h"
#include"inode.h"
#include"str.h"
#include"dic_entry.h"
#include"elf.h"
#include"sec_header.h"
#include"jump.h"

#define FS_START_LBA 128
#define FS_SIGNATURE 0xAF01
#define FS_BUFFER 0x1400000
#define BLOCK_SECTOR_CNT 8
#define INODE_PER_SECTOR 2
#define SECTOR_SIZE 512
#define BLOCK_SIZE 4096
#define KERNEL_NAME "arcus_kerenl"

void loader_main() __attribute__ ((section (".text.main"))); // make the loader_main function at the beginning of the .text section
uint64 get_lba_by_dbnum();
void find_kernel_file_and_read();
void load_kernel_from_disk(uint64 ind_num);
void load_elf_kernel(uint8* kaddr, uint32 cpu_id);
uint32 get_apic_id();

static struct superblock* spblock = (struct superblock*)(FS_BUFFER);
static struct block_group_desc* gpdesc_ptr = (struct block_group_desc*)(FS_BUFFER + BLOCK_SIZE);
static struct inode* ind = (struct inode*)(FS_BUFFER + BLOCK_SIZE + SECTOR_SIZE);
static struct dic_entry* dic_ptr = (struct dic_entry*)(FS_BUFFER + BLOCK_SIZE + SECTOR_SIZE * 2);
static uint8* tmp_buffer = (uint8*)(FS_BUFFER + BLOCK_SIZE * 2 + SECTOR_SIZE * 2);
static uint8* tmp_buffer2 = (uint8*)(FS_BUFFER + BLOCK_SIZE * 3 + SECTOR_SIZE * 2);

const uint64 KERNEL_FILE_BASE = 0xa00000;
static int32 bsp_cpu_id __attribute__((section(".data"))) = -1;
static uint64 kernel_entry __attribute__((section(".data"))) = 0;

void loader_main() {
    // bsp_cpu_id为-1，说明是第一次执行这段代码，进入这段代码的cpu为bsp，加载内核并对bsp赋值
    if (bsp_cpu_id == -1) {
        bsp_cpu_id = get_apic_id();
        find_kernel_file_and_read(bsp_cpu_id);
    }    
    // bsp_cpu_id不为-1，说明进入这段代码的cpu是ap，直接跳转到内核入口
    jump_to_kernel(kernel_entry);
}

void find_kernel_file_and_read(uint32 cpu_id) {
    ld_read_disk(FS_START_LBA, 8, FS_BUFFER);
    if (spblock->signature != FS_SIGNATURE) {
        ld_print("Filesystem not found!");
        asm("hlt");    
    }    
    ld_read_disk(FS_START_LBA + 8, 1, gpdesc_ptr);

    ld_read_disk(gpdesc_ptr->inode_table_lba, 1, ind);
    ld_read_disk(get_lba_by_dbnum(ind->pt_direct[0]), BLOCK_SECTOR_CNT, dic_ptr);
    for (int i = 0; i < 8; i++) {
        if (dic_ptr[i].inode_num == 0) {
            break;
        }
        if (!ld_strcmp(KERNEL_NAME, dic_ptr[i].name)) {
            ld_print("Find kernel: /%s inode: %d \n", dic_ptr[i].name, dic_ptr[i].inode_num);
            load_kernel_from_disk(dic_ptr[i].inode_num);
            load_elf_kernel(KERNEL_FILE_BASE, cpu_id);
        }
    }
}

void load_kernel_from_disk(uint64 ind_num) {
    ld_read_disk(gpdesc_ptr->inode_table_lba + (ind_num - 1) / INODE_PER_SECTOR , 1, ind);
    int offset = (ind_num - 1) % INODE_PER_SECTOR;
    int tot_block = ind[offset].tot_size / BLOCK_SIZE;
    uint8* base = (uint8*)KERNEL_FILE_BASE;
    int j = 0;
    while (j < tot_block) {
        if (j < 12) {
            ld_read_disk(get_lba_by_dbnum(ind[offset].pt_direct[j++]), 8, base);
            base += BLOCK_SIZE;
        } else if (j < 12 + 4096 / 8) {
            ld_read_disk(get_lba_by_dbnum(ind[offset].pt1), 8, tmp_buffer);
            uint64* dnum_ptr = (uint64*)tmp_buffer;
            int k = 0;
            while (j < tot_block && j < 12 + 4096 / 8) {
                ld_read_disk(get_lba_by_dbnum(dnum_ptr[k++]), 8, base);
                base += BLOCK_SIZE;
                j++;
            }
        } else if (j < 12 + (4096 / 8) * (4096 / 8)) {
            ld_read_disk(get_lba_by_dbnum(ind[offset].pt2), 8, tmp_buffer);
            uint64* dnum_ptr = (uint64*)tmp_buffer;
            for (int k = 0; k < 4096 / 8; k++) {
                ld_read_disk(get_lba_by_dbnum(dnum_ptr[k]), 8, tmp_buffer2);
                uint64* dnum_ptr1 = (uint64*)tmp_buffer2;
                int finished = 0;
                int l = 0;
                while (j < tot_block && j < 12 + (4096 / 8) * (4096 / 8) && l < 4096 / 8) {
                    ld_read_disk(get_lba_by_dbnum(dnum_ptr1[l++]), 8, base);
                    base += BLOCK_SIZE;
                    j++;
                }
                if (j >= tot_block) {
                    break;
                }
            }
        }
    }
}

void load_elf_kernel(uint8* kaddr, uint32 cpu_id) {
    uint8* tmp = (uint8*)KERNEL_FILE_BASE;
    struct elf_head* ehead = (struct elf_head*) KERNEL_FILE_BASE;
    if (ehead->e_ident[0] != 0x7f || ehead->e_ident[1] != 'E' || ehead->e_ident[2] != 'L' || ehead->e_ident[3] != 'F') {
        ld_print("Unsupported ABI! %d %d %d %d", ehead->e_ident[0], ehead->e_ident[1], ehead->e_ident[2], ehead->e_ident[3]);
        asm("hlt");     
    }

    ld_print("Start load ELF Kernel. Section count: %d. Program entry: %l\n", ehead->e_shnum, ehead->e_entry);
    struct elf_shdr* shdr_ptr = (struct elf_shdr*)(ehead->e_shoff + tmp);
    for (int i = 0; i < ehead->e_shnum; i++) {
        uint8* target = (uint8*)shdr_ptr[i].sh_addr;
        uint8* src = tmp + shdr_ptr[i].sh_offset;
        for (int k = 0; k < shdr_ptr[i].sh_size; k++) {
            target[k] = src[k];
        }
    }
    kernel_entry = ehead->e_entry;
    jump_to_kernel(ehead->e_entry);
}

uint64 get_lba_by_dbnum(uint64 dnum) {
    return gpdesc_ptr->data_block_lba + (dnum - 1) * BLOCK_SECTOR_CNT;
}
