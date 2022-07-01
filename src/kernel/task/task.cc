#include"task/task.h"
#include"mem/slab.h"
#include"printk.h"

using namespace arcus::memory;
namespace arcus::task
{
    task_struct root_task;

    /**
     * 任务调度，由时钟中断触发
     */
    void schedule() {
    }

    void init_task() {
        root_task.name = "kerenl_task";
        root_task.pid = 0;
        root_task.stack = kmalloc(TASK_STACK_SIZE);
    }
}