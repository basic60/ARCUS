#ifndef __ARCUS_INCLUDE_INIT
#define __ARCUS_INCLUDE_INIT
#include"compiler_attribute.h"
namespace arcus
{
    #define __init __section(.init.text)
    #define __initdata __section(.init.data)
    #define __initconst	__section(.init.rodata)
    void init_kernel() __init;

}
#endif