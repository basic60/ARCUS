#include"interrupt/idt.h"
#include"mem/mem_block.h"
#include"str.h"
#include"printk.h"
#include"io/port.h"
using namespace arcus::memory;

extern "C" void idt_flush(arcus::uint64 addr);

namespace arcus::interrupt
{
    static idtr idt_reg;
    // 中断向量表，指向汇编中断处理程序入口
    static idt_entry isr_vector[256];
    static void (**int_handler)(interrupt_stack* data);

    static inline void set_idt_entity(int int_num, uint64 isr_addr, uint16 selector, uint8 flags) {
        isr_vector[int_num].offset_low = isr_addr & 0xffff;
        isr_vector[int_num].offset_middle = (isr_addr >> 16) & 0xffff;
        isr_vector[int_num].offset_high = (isr_addr >> 32) & 0xffffffff;
        isr_vector[int_num].ist = 0;
        isr_vector[int_num].segment = selector;
        isr_vector[int_num].type = flags;
        isr_vector[int_num].reserve = 0;
    }

    void init_interrupt() {
        memset(isr_vector, 0, sizeof(idt_entry) << 8);
        int_handler = (void (**)(interrupt_stack*)) mblock_allocate(sizeof(int_handler) << 8, 8);
        memset(int_handler, 0, sizeof(int_handler) << 8);

        // 初始化主片、从片 // 0001 0001 
        io::write_port(0x20, 0x11); 
        io::write_port(0xA0, 0x11);
        // 设置主片 IRQ 从 0x20(32) 号中断开始
        io::write_port(0x21, 0x20);
        // 设置从片 IRQ 从 0x28(40) 号中断开始
        io::write_port(0xA1, 0x28);

        // 设置主片 IR2 引脚连接从片 
        io::write_port(0x21, 0x04);
        // 告诉从片输出引脚和主片 IR2 号相连 
        io::write_port(0xA1, 0x02);
        
        // 设置主片和从片按照 8086 的方式工作 
        io::write_port(0x21, 0x01);
        io::write_port(0xA1, 0x01);
        
        // 设置主从片允许中断 
        io::write_port(0x21, 0x0);
        io::write_port(0xA1, 0x0);

        set_idt_entity( 0, (uint64)  isr0, 0x08, 0x8E);
        set_idt_entity( 1, (uint64)  isr1, 0x08, 0x8E);
        set_idt_entity( 2, (uint64)  isr2, 0x08, 0x8E);
        set_idt_entity( 3, (uint64)  isr3, 0x08, 0x8E);
        set_idt_entity( 4, (uint64)  isr4, 0x08, 0x8E);
        set_idt_entity( 5, (uint64)  isr5, 0x08, 0x8E);
        set_idt_entity( 6, (uint64)  isr6, 0x08, 0x8E);
        set_idt_entity( 7, (uint64)  isr7, 0x08, 0x8E);
        set_idt_entity( 8, (uint64)  isr8, 0x08, 0x8E);
        set_idt_entity( 9, (uint64)  isr9, 0x08, 0x8E);
        set_idt_entity(10, (uint64) isr10, 0x08, 0x8E);
        set_idt_entity(11, (uint64) isr11, 0x08, 0x8E);
        set_idt_entity(12, (uint64) isr12, 0x08, 0x8E);
        set_idt_entity(13, (uint64) isr13, 0x08, 0x8E);
        set_idt_entity(14, (uint64) isr14, 0x08, 0x8E);
        set_idt_entity(15, (uint64) isr15, 0x08, 0x8E);
        set_idt_entity(16, (uint64) isr16, 0x08, 0x8E);
        set_idt_entity(17, (uint64) isr17, 0x08, 0x8E);
        set_idt_entity(18, (uint64) isr18, 0x08, 0x8E);
        set_idt_entity(19, (uint64) isr19, 0x08, 0x8E);
        set_idt_entity(20, (uint64) isr20, 0x08, 0x8E);
        set_idt_entity(21, (uint64) isr21, 0x08, 0x8E);
        set_idt_entity(22, (uint64) isr22, 0x08, 0x8E);
        set_idt_entity(23, (uint64) isr23, 0x08, 0x8E);
        set_idt_entity(24, (uint64) isr24, 0x08, 0x8E);
        set_idt_entity(25, (uint64) isr25, 0x08, 0x8E);
        set_idt_entity(26, (uint64) isr26, 0x08, 0x8E);
        set_idt_entity(27, (uint64) isr27, 0x08, 0x8E);
        set_idt_entity(28, (uint64) isr28, 0x08, 0x8E);
        set_idt_entity(29, (uint64) isr29, 0x08, 0x8E);
        set_idt_entity(30, (uint64) isr30, 0x08, 0x8E);
        set_idt_entity(31, (uint64) isr31, 0x08, 0x8E);

        set_idt_entity(32, (uint64) isr32, 0x08, 0x8E);
        set_idt_entity(33, (uint64) isr33, 0x08, 0x8E);
        set_idt_entity(34, (uint64) isr34, 0x08, 0x8E);
        set_idt_entity(35, (uint64) isr35, 0x08, 0x8E);
        set_idt_entity(36, (uint64) isr36, 0x08, 0x8E);
        set_idt_entity(37, (uint64) isr37, 0x08, 0x8E);
        set_idt_entity(38, (uint64) isr38, 0x08, 0x8E);
        set_idt_entity(39, (uint64) isr39, 0x08, 0x8E);
        set_idt_entity(40, (uint64) isr40, 0x08, 0x8E);
        set_idt_entity(41, (uint64) isr41, 0x08, 0x8E);
        set_idt_entity(42, (uint64) isr42, 0x08, 0x8E);
        set_idt_entity(43, (uint64) isr43, 0x08, 0x8E);
        set_idt_entity(44, (uint64) isr44, 0x08, 0x8E);
        set_idt_entity(45, (uint64) isr45, 0x08, 0x8E);
        set_idt_entity(46, (uint64) isr46, 0x08, 0x8E);
        set_idt_entity(47, (uint64) isr47, 0x08, 0x8E);
        set_idt_entity(64, (uint64) isr47, 0x08, 0x8E);

        idt_reg.limit = sizeof(idt_entry) * 256 - 1;
        idt_reg.idt = (uint64) &isr_vector;
        idt_flush((uint64) &idt_reg);
    }

    void isr_dispatcher(interrupt_stack* data) {
    }

    void isr_pic_dispatcher(interrupt_stack* data) {
        if (data->int_num >= 40) 
            // 发送重设信号给从片
            io::write_port(0xA0, 0x20);
        // 发送重设信号给主片
        io::write_port(0x20, 0x20);
    }

}