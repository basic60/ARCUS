#include"str.h"
#include"types.h"
namespace arcus
{
    size_t strlen(const char* s) {
        size_t result = 0;
        while (s[result++]);
        return result - 1;
    }

    void memset(void* addr, uint8 d, size_t len) {
        size_t idx = 0;
        while (len) {
            ((char*)addr)[--len] = d;
        }
    }
}
