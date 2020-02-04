#include"data_block.h"
#include<cstring>
using namespace artools;

void data_block::clear() {
    memset(data, 0, sizeof(data_block));
}
