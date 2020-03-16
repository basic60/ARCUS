#ifndef __INCLUDE_ARCUS_BOOTLOADER_STR
#define __INCLUDE_ARCUS_BOOTLOADER_STR

int ld_strlen(const char* s);

int ld_strcmp(const char* src, const char* dst);

void ld_memcpy(char* dst, char* src, int len);

#endif