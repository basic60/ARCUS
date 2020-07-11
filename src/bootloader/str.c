#include"str.h"
int ld_strlen(const char* s) {
    int len = 0;
    while (s[len] != '\0') {len++;}
    return len;
}

int ld_strcmp(const char* src, const char* dst) {
    int i = 0;
    while (1) {
        if (src[i] == dst[i] && src[i] == '\0') {
            return 0;
        } else if(src[i] == dst[i]) {
            i++;
        } else {
            return src[i] > dst[i];
        }
    }
    return 0;
}