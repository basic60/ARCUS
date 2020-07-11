#ifndef __ARCUS_INCLUDE_VAARGS
#define __ARCUS_INCLUDE_VAARGS
typedef __builtin_va_list va_list;
#define va_start(ap, last) (__builtin_va_start(ap, last))
#define va_arg(ap, type) (__builtin_va_arg(ap, type))
#define va_end(ap)
#endif