#include<iostream>
#include<sys/mman.h>
#include<fcntl.h>
#include<unistd.h>
#include <sys/stat.h>
#include"controller.h"
using namespace artools;

#define MIN_DISK_SIZE 150*1024*1024

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout<<"Usage: ./mkmyfs <image_path> <param>"<<std::endl;
        exit(-1);
    }

    int fd;
    struct stat stat_info;

    fd = open(argv[1], O_RDWR, NULL);
    if (fd < 0) {
        std::cerr<<"Open file error."<<std::endl;
        exit(-1);
    }

    fstat(fd, &stat_info);
    if (stat_info.st_size < MIN_DISK_SIZE) {
        std::cerr<<"Image should be at least 150MB."<<std::endl;
        close(fd);
        exit(-1);
    }

    void* addr = mmap(NULL, stat_info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == (void*)-1) {
        std::cerr<<"Map file error!"<<std::endl;
        exit(-1);
    }

    std::shared_ptr<controller> ctr(new controller((uint8*)addr));
    if (strcmp(argv[2], "-f") == 0) {
        ctr->format(stat_info.st_size);
    } else if(strcmp(argv[2], "-r") == 0) {
        ctr->print_spblock();
        ctr->ls_root();
    } else if(strcmp(argv[2], "-w") == 0) { // write the kernel to path "/"
        if (argc < 4) {
            std::cout<<"Please specify the file and path in order to write in the image."<<std::endl;
            exit(-1);
        }

        int fdk;
        const char* fname = argv[3];
        fdk = open(fname, O_RDONLY, NULL);
        if (fdk < -1) {
            std::cerr<<"Open image "<<fname<<" failed!"<<std::endl;
            close(fd);
            exit(-1);
        }
        struct stat stat_info_img;
        fstat(fdk, &stat_info_img);

        void* addr_kernel = mmap(NULL, stat_info_img.st_size, PROT_READ, MAP_SHARED, fdk, 0);
        if (addr_kernel == (void*)-1) {
            std::cerr<<"Map file error!"<<std::endl;
            exit(-1);
        }

        ctr->write_into_kernel((uint8*)addr_kernel, stat_info_img.st_size);
        close(fdk);
    }

    munmap(addr, stat_info.st_size);
    close(fd);
    std::cout<<"finished!"<<std::endl;
    return 0;
}
