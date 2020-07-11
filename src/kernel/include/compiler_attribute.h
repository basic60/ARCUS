#ifndef __ARCUS_INCLUDE_COMPILER_ATTRIBUTE
#define __ARCUS_INCLUDE_COMPILER_ATTRIBUTE
#define __section(S)  __attribute__((__section__(#S)))
#endif