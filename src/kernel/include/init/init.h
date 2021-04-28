#ifndef __ARCUS_INCLUDE_INIT
#define __ARCUS_INCLUDE_INIT
#include"compiler_attribute.h"
namespace arcus
{
    // 标记初始化所用的数据段、代码段，内核初始化完成后回收初始化代码和数据所占用的内存空间。
    #define __init __section(.init.text)
    #define __initdata __section(.init.data)
    #define __initconst	__section(.init.rodata)
    void init_kernel() __init;

}
#endif