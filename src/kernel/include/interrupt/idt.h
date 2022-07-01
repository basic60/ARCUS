#ifndef __ARCUS_INCLUDE_INTERRUPT_IDT
#define __ARCUS_INCLUDE_INTERRUPT_IDT
#include"types.h"
#include"init/init.h"
namespace arcus::interrupt
{
    struct idtr {
        uint16 limit;
        // idt表基地址
        uint64 idt;
    }__attribute__((packed));

    struct idt_entry {
        uint16 offset_low;
        uint16 segment;
        uint8 ist;
        uint8 type;
        uint16 offset_middle;
        uint32 offset_high;
        uint32 reserve;
    }__attribute__((packed));

    /**
     * 中断发生时栈中的数据
     */
    struct interrupt_stack {
        uint64 ds;  
        uint64 r15; 
        uint64 r14;  
        uint64 r13;  
        uint64 r12;  
        uint64 r11; 
        uint64 r10; 
        uint64 r9;
        uint64 r8;  
        uint64 rdi;
        uint64 rsi;
        uint64 rbp;
        uint64 rdx;
        uint64 rcx;
        uint64 rbx;
        uint64 rax; 
        uint64 int_num;
        uint64 err_code;
        uint64 rip;
        uint64 cs;
        uint64 rflags;
        uint64 user_rsp;
        uint64 ss;
    };

    extern "C" void isr_dispatcher(interrupt_stack* data);
    extern "C" void isr_pic_dispatcher(interrupt_stack* data);

    void init_interrupt() __init;
    // 0 #DE 除 0 异常
    extern "C" void isr0();
    // 1 #DB 调试异常
    extern "C" void isr1();
    // 2 NMI
    extern "C" void isr2();
    // 3 BP 断点异常
    extern "C" void isr3();
    // 4 #OF 溢出
    extern "C" void isr4();
    // 5 #BR 对数组的引用超出边界
    extern "C" void isr5();
    // 6 #UD 无效或未定义的操作码
    extern "C" void isr6();
    // 7 #NM 设备不可用无数学协处理器()
    extern "C" void isr7();
    // 8 #DF 双重故障有错误代码()
    extern "C" void isr8();
    // 9 协处理器跨段操作
    extern "C" void isr9();
    // 10 #TS 无效TSS有错误代码()
    extern "C" void isr10();
    // 11 #NP 段不存在有错误代码()
    extern "C" void isr11();
    // 12 #SS 栈错误有错误代码()
    extern "C" void isr12();
    // 13 #GP 常规保护有错误代码()
    extern "C" void isr13();
    extern "C" void isr14();
    extern "C" void isr15();
    extern "C" void isr16();
    extern "C" void isr17();
    extern "C" void isr18();
    extern "C" void isr19();
    // 20 ~ 31 Intel 保留
    extern "C" void isr20();
    extern "C" void isr21();
    extern "C" void isr22();
    extern "C" void isr23();
    extern "C" void isr24();
    extern "C" void isr25();
    extern "C" void isr26();
    extern "C" void isr27();
    extern "C" void isr28();
    extern "C" void isr29();
    extern "C" void isr30();
    extern "C" void isr31();
    
    // 电脑系统计时器
    extern "C" void isr32();
    // 键盘
    extern "C" void isr33();
    // 与 IRQ9 相接，MPU-401 MD 使用
    extern "C" void isr34();
    // 串口设备
    extern "C" void isr35();
    // 串口设备
    extern "C" void isr36();
    // 建议声卡使用
    extern "C" void isr37();
    // 软驱传输控制使用
    extern "C" void isr38();
    // 打印机传输控制使用
    extern "C" void isr39();
    // 即时时钟
    extern "C" void isr40();
    // 与 IRQ2 相接，可设定给其他硬件
    extern "C" void isr41();
    // 建议网卡使用
    extern "C" void isr42();
    // 建议 AGP 显卡使用
    extern "C" void isr43();
    // 接 PS/2 鼠标，也可设定给其他硬件
    extern "C" void isr44();
    // 协处理器使用
    extern "C" void isr45();
    // IDE0 传输控制使用
    extern "C" void isr46();
    // IDE1 传输控制使用
    extern "C" void isr47();

    void bind_int_handler(int int_id, void (*)(interrupt_stack* data));
}
#endif