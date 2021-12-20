#ifndef __ARCUS_INCLUDE_KTYPES
#define __ARCUS_INCLUDE_KTYPES
namespace arcus
{
    typedef unsigned char uint8;
    typedef char int8;
    typedef unsigned short uint16;
    typedef short int16;
    typedef unsigned int uint32;
    typedef int int32;
    typedef unsigned long long uint64;
    typedef long long int64;
    typedef unsigned long long size_t;

    #define OFFSET_OF(type, member) ((size_t) &((type*) 0)->member)
    // 最后一行statement的值为复合表达式的值，不属于C++标准，但属于GCC支持的扩展功能。  
    #define CONTAINER_OF(ptr, type, member) ({ const decltype(((type*) 0)->member)* __tmp_ptr = (ptr);\
        (type*)(__tmp_ptr - OFFSET_OF(type, member)); }) 
}
#endif