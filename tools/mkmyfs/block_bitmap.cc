#include"block_bitmap.h"
#include<cstring>
using namespace artools;

bool block_bitmap::get_index(int index) {
    if (index >= 4096 * 8) {
        return 0;
    }

    uint8 cell = bits[index / 8];
    return (cell >> index % 8) & 1;
}

void block_bitmap::set_index(int index, bool value) {
    if (index >= 4096 * 8) {
        return;
    }

    uint8 cell = bits[index / 8];
    cell |=  1 << (index % 8);
}

int block_bitmap::poll() {
    for (int i= 0; i < 4096; i++) {
        if (!get_index(i)) {
            set_index(i, true);
            return i;
        }
    }
    return -1;
}

void block_bitmap::clear() {
    memset(bits, 0, sizeof(bits));
}