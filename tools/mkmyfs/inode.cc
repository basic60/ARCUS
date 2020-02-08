#include"inode.h"
#include<cstring>
#include<sys/time.h>
using namespace artools;



inode::inode(uint32 t_uid, uint32 t_gid, uint16 md, uint32 ftp) {
    memset(this, 0 ,sizeof(inode));

    struct timeval tp;
    gettimeofday(&tp, NULL);
    long long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    this->atime = ms;
    this->ctime = ms;
    this->mtime = ms;

    this->link_count = 1;

    this->uid = t_uid;
    this->gid = t_gid;
    this->mode = md;
    this->file_type = ftp;
}
