#include"interrupt/timer.h"
#include"interrupt/idt.h"
#include"io/port.h"
#include"task/task.h"
#include"printk.h"
#include"cpu/ap.h"

namespace arcus::interrupt
{
    static void handle_schedule();
    static void add_running_time();
    static uint64 ms;

    void timer_handler(interrupt_stack* data) {
        add_running_time();
        handle_schedule();
    }

    void init_timer(uint32 frequency) {
        ms = 0;
        asm __volatile__("cli");
        // 注册时间相关的处理函数 
        bind_int_handler(32, timer_handler);
        // Intel 8253/8254 芯片PIT I/端口地址范围是O40h~43h 
        // 输入频率为， 1193180 / frequency 即每秒中断次数 
        uint32 divisor = 1193180 / __INTERRUPT_TIMER_FREQUENCY;
        // D7 D6 D5 D4 D3 D2 D1 D0
        // 0 0 1 1 0 1 1 0
        // 即就是 36 H
        // 设置 8253/8254 芯片工作在模式 3 下
        io::write_port(0x43, 0x36);

        // 拆分低字节和高字节
        uint8 low = (uint8)(divisor & 0xFF);
        uint8 high = (uint8)((divisor >> 8) & 0xFF);
        // reload 
        io::write_port(0x40, low);     
        io::write_port(0x40, high);
    }

    uint64 get_runing_ms() {
        return ms;
    }

    static void handle_schedule() {
        task::schedule();
    }

    static void inline add_running_time() {
        ms += 1000 / __INTERRUPT_TIMER_FREQUENCY;
    }
}