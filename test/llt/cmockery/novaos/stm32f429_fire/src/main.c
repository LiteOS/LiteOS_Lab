#include <config.h>
#include <stdint.h>
#include <stddef.h>

#ifdef CONFIG_KPRINTF
#include <kprintf.h>
#endif

int main (void)
    {

    /*
     * main task created for you, please add your code here
     *
     * NOTE: the arguments used to create this task
     *
     *  +------------+----------------------+---------+
     *  | attribute  | kconfig entry name   | default |
     *  +------------+----------------------+---------+
     *  | priority   | MAIN_TASK_PRIO       | 20      |
     *  | stack size | MAIN_TASK_STACK_SIZE | 0x800   |
     *  | options    | MAIN_TASK_OPTIONS    | 0       |
     *  +------------+----------------------+---------+
     *
     * you can change them by using the project tool (`prj -s` or `prj -m`)
     * 
     * of cause, you can do your initialization here and just let this task
     * return, and this task will be terminated
     */

#ifdef CONFIG_KPRINTF
    kprintf ("hello world :-)\n");
#endif

    extern int link_main(void *args);

    link_main(NULL);

    return 0;
    }
