#include"task/task.h"
#include"mem/slab.h"
#include"printk.h"
#include"interrupt/timer.h"

using namespace arcus::memory;
namespace arcus::task
{
    task_struct* cur_task;
    static int pid;

    /**
     * 任务调度，由时钟中断触发
     */
    void schedule() {
        if (cur_task->list.next == &cur_task->list) {
            if (cur_task->sleep_time > 0) {
                cur_task->sleep_time -= 1000 / __INTERRUPT_TIMER_FREQUENCY;
            }
        }
        task_struct* next_task = LIST_ENTRY(cur_task->list.next, task_struct, list);
        task_struct* origin_task = cur_task;
        cur_task = next_task;
        switch_task(&origin_task->context, &next_task->context);
    }

    void init_task() {
        pid = 0;
        cur_task = (task_struct*) kmalloc(sizeof(task_struct));
        cur_task->name = "kerenl_task";
        cur_task->pid = pid;
        cur_task->list.prev = &cur_task->list;
        cur_task->list.next = &cur_task->list;
    }

    void kthread_create(int (*fn)(void*), void* arg, const char* task_name) {
        task_struct* new_task = (task_struct*) kmalloc(TASK_STACK_SIZE);
        new_task->name = task_name;
        new_task->pid = ++pid;
        uint64* stack_top = (uint64*)((uint64)new_task + TASK_STACK_SIZE);
        *(--stack_top) = (uint64) arg;
        *(--stack_top) = (uint64) fn;

        new_task->context.rsp = (uint64) stack_top;
        // 设置新任务的标志寄存器未屏蔽中断
        new_task->context.rflags = 0x200;

        list_add_after(&new_task->list, cur_task->list.prev);
    }

    void sleep(uint64 ms) {
        cur_task->sleep_time = interrupt::get_runing_ms() + ms;
        while (1) {
            if (interrupt::get_runing_ms() > cur_task->sleep_time)
                break;
        }   
    }

}