#include"printk.h"
#include"va_args.h"
#include"str.h"
#include"cpu/spinlock.h"
namespace arcus
{
    #define COLOR_WHITE 0x07
    #define MAX_PRINTK_BUF_LEN 1024
    static unsigned short *vmem = (unsigned short*)(0xB8000);
    static const int SCREEN_WIDTH = 80;
    static const int SCREEN_HEIGHT = 25;

    static int idx __attribute__((section(".data"))) = 0; // 不指明在.data段，则0会导致放入.bss段。objcopy不会复制.bss段，导致idx不确定
    static cpu::spinlock __attribute__((section(".data"))) print_lock = {0, 0};

    static void scroll_text_screen() {
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

    static void putc(char c) {
        if (idx >= SCREEN_WIDTH * SCREEN_HEIGHT)  {
            scroll_text_screen();
        }
        vmem[idx++] = (COLOR_WHITE << 8) | c;
    }

    void putn() {
        if (idx >= (SCREEN_HEIGHT - 1) * SCREEN_WIDTH) {
            scroll_text_screen();
        } else {
            idx = idx + SCREEN_WIDTH - (idx % SCREEN_WIDTH);
        }
    }

    static void print_str(const char* s) {
        int len = strlen(s);
        for (size_t i = 0; i < len; i++) {
            putc(s[i]);
        }
    }

    static void print_decimal(uint64 d) {
        char print_buf[50];
        int len = 0;
        bool ng = 0;
        if(d < 0) {
            ng = 1;
            d *= -1;
        }

        if (d == 0) {
            print_buf[len++] = '0';
        } else {
            while (d) {
                print_buf[len++] = d % 10 + '0';
                d /= 10;
            }
        }
        if (ng) putc('-');
        for (int i = len - 1; i >= 0; i--) {
            putc(print_buf[i]);
        }
    }

    static void print_hex(uint64 d) {
        char print_buf[50];
        int len = 0;
        bool ng = 0;
        if(d < 0) {
            ng = 1;
            d *= -1;
        }
        if (d == 0) {
            print_buf[len++] = '0';
        } else {
            while (d) {
                print_buf[len++] = d % 16 < 10 ? '0' + d % 16 : 'a' + d % 16 - 10;
                d /= 16;
            }
        }
        if (ng) putc('-');
        putc('0');putc('x');
        for (int i = len - 1; i >= 0; i--) {
            putc(print_buf[i]);
        }
    }

    static void print_num(int64 val, bool u_sign, bool hex) {
        if (val < 0 && !u_sign) {
            val *= -1;
            putc('-');
        }
        if (hex) {
            print_hex(val);
        } else {
            print_decimal(val);
        }
    }

    void printk(const char* s, ...) {
        va_list plist;
        va_start(plist, s);
        pt_state state = start;
        size_t len = strlen(s);
        if (len > MAX_PRINTK_BUF_LEN) {
            return;
        }

        cpu::spin_lock(&print_lock);
        bool fmt = 0;
        bool fm_long = 0;
        bool fm_unsigned = 0;
        for (size_t i = 0; i < len; i++) {
            char c = s[i];
            if (fmt && c == '%') {
                putc(c);
                fmt = 0;
            } else if (fmt && c == 'd') {
                int tmp = va_arg(plist, int);
                print_decimal(tmp);
                fmt = 0;
            } else if (fmt && c == 'x') {
                int tmp = va_arg(plist, int);
                print_hex(tmp); 
                fmt = 0;
            } else if (fmt && c == 'l') {
                fmt = 0;
                fm_long = 1;
            } else if (fmt && c == 'u') {
                fm_unsigned = 1;
            } else if (fm_long && c == 'u') {
                fm_unsigned = 1;
            } else if (fm_long && c == 'd') {
                int64 tmp = va_arg(plist, long long);
                print_num(tmp, fm_unsigned, false);
                fm_long = fm_unsigned = 0;
            } else if (fm_long && c == 'x') {
                int64 tmp = va_arg(plist, long long);
                print_num(tmp, fm_unsigned, true);
                fm_long = fm_unsigned = 0;
            } else if (fmt && c == 'c') {
                putc(va_arg(plist, int));
                fmt = 0;
            } else if (fmt && c == 's') {
                const char* tmp = va_arg(plist, char*);
                print_str(tmp);
                fmt = 0;
            } else if (c == '\n') {
                putn();
            } else if (c == '\t') {
                putc(' ');putc(' ');putc(' ');putc(' ');
            } else if (c == '%') {
                fmt = 1;
            } else {
                putc(c);
            }
        }
        va_end(plist);
        cpu::spin_unlock(&print_lock);
    }

	void clear_text_screnn() {
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                vmem[i * SCREEN_WIDTH + j] = (COLOR_WHITE << 8) | ' ';
            }
        }
    }
}
