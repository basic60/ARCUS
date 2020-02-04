#include"dic_entry.h"
#include<cstring>
using namespace artools;
dic_entry::dic_entry() {
    name_len = inode_num = 0;
    memset(name, 0, sizeof(name));
}

dic_entry::dic_entry(const char* src, uint64 inum) {
    this->name_len = strlen(src);
    strcpy(this->name, src);
    this->inode_num = inum;
}