#include "print.h"
#include"str.h"
#include"va_args.h"
int idx __attribute__((section(".data"))) = 0; // 不指明在.data段则会放入.bss段。objcopy不会复制.bss段，导致idx不确定
static char tmp_buffer[50];
static unsigned short *vmem = (unsigned short*)(0xB8000);
static const int SCREEN_WIDTH = 80;
static const int SCREEN_HEIGHT = 25;


void ld_print(const char *pattern, ...)
{
    va_list plist;
    va_start(plist, pattern);

    int len = ld_strlen(pattern);
    int flag = 0;
    for (int i = 0; i < len; i++) {
        if (flag) {
            if (pattern[i] == '%') {
                ld_putc('%');
                flag = 0;
                continue;
            }
            switch (pattern[i]) {
            case 'd': {
                int tmp = va_arg(plist, int);
                int sz = 0;
                if (tmp == 0) {
                    sz = 1;
                    tmp_buffer[0] = '0';
                } else {
                    while (tmp) {
                        tmp_buffer[sz++] = (tmp % 10) + '0';
                        tmp /= 10;
                    }
                }
                for (int j = sz - 1; j >= 0; j--) {
                    ld_putc(tmp_buffer[j]);
                }
                break;
            }
            case 'l': {
                long long tmp = va_arg(plist, long long);
                int sz = 0;
                if (tmp == 0) {
                    sz = 1;
                    tmp_buffer[0] = '0';
                } else {
                    while (tmp) {
                        tmp_buffer[sz++] = (tmp % 10) + '0';
                        tmp /= 10;
                    }
                }
                for (int j = sz - 1; j >= 0; j--) {
                    ld_putc(tmp_buffer[j]);
                }
                break;
            } case 'c': {
                char tmp = va_arg(plist, int);
                ld_putc(tmp);
                break;
            } case 's': {
                const char* tmp = va_arg(plist, const char*);
                int idx = 0;
                while (tmp[idx] != '\0') {
                    ld_putc(tmp[idx++]);
                }
                break;
            }
            default:
                break;
            }

            flag = 0;
        } else {
            if (pattern[i] == '%') {
                flag = 1;
                continue;
            } else if (pattern[i] == '\n') {
                ld_putn();
                continue;
            }
            ld_putc(pattern[i]);
        }
    }
    va_end(plist);
}

void ld_putc(char c) {
    if (idx + 1 >= SCREEN_WIDTH * SCREEN_HEIGHT)  {
        ld_scroll();
    }
    vmem[idx++] = (COLOR_WHITE << 8) | c;
}

void ld_putn() {
    if (idx >= (SCREEN_HEIGHT - 1) * SCREEN_WIDTH) {
        ld_scroll();
    } else {
        idx = idx + SCREEN_WIDTH - (idx % SCREEN_WIDTH);
    }
}

void ld_scroll() {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            if (i < SCREEN_HEIGHT - 1) {
                vmem[i * SCREEN_WIDTH + j] = vmem[(i + 1) * SCREEN_WIDTH + j];
            } else {
                vmem[i * SCREEN_WIDTH + j] = 0;
            }
        }
    }
    idx = SCREEN_WIDTH * (SCREEN_HEIGHT - 1);
}
