#ifndef __ARCUS_TOOLS_BLOCK_BITMAP_H
#define __ARCUS_TOOLS_BLOCK_BITMAP_H
#include"types.h"
namespace artools
{
    class block_bitmap {
    private:
        uint8 bits[4096];
    public:
        int poll();
        void set_index(int index, bool value);
        bool get_index(int index);
        void clear();
    };
}
#endif
