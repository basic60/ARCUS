#ifndef __ARCUS_INCLUDE_TASK
#define __ARCUS_INCLUDE_TASK
#include"types.h"
#include"list.h"
#include"init/init.h"

namespace arcus::task
{
    #define TASK_STACK_SIZE 8192

    struct task_struct {
        // 仅用于展示进程信息使用
        const char* name;
        // 进程id
        uint32 pid;
        // 进程栈
        void* stack;
        // 进程上下文
        list_head list;

        struct task_context {
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
            uint64 rflags;
            uint64 rsp;
        }__attribute__((packed)) context;

        list_head children;
        uint64 start_time;
    
    };

    void __init init_task();

    void schedule();

    extern "C" void switch_task(task_struct::task_context* cur, task_struct::task_context* next);
}
#endif