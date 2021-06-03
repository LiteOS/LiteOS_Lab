/* 使用osal接口需要包含该头文件 */
#include <osal.h>

/* 任务优先级宏定义（shell任务的优先级为10） */
#define USER_TASK1_PRI  12  //低优先级
#define USER_TASK2_PRI  11  //高优先级

/* 任务ID */
uint32_t user_task1_id = 0;
uint32_t user_task2_id = 0;

/* 任务task1入口函数 */
static int user_task1_entry()
{
    int n = 0;

    /* 每隔2s在串口打印一次，打印5次后主动结束 */
    for(n = 0; n < 5; n++)
    {
        printf("task1: my task id is %ld, n = %d!\r\n", user_task1_id, n);

        /* 任务主动挂起2s */
        osal_task_sleep(2*1000);
    }

    printf("user task 1 exit!\r\n");

    /* 任务结束 */
    return 0;
}
/* 任务task2入口函数 */
static int user_task2_entry()
{
    /* 每隔2s在串口打印一次，不结束 */
    while (1)
    {
        printf("task2: my task id is %ld!\r\n", user_task2_id);

        /* 任务主动挂起2s */
        osal_task_sleep(2*1000);
    }
    return 0;
}

/* 标准demo启动函数，函数名不要修改，否则会影响下一步实验 */
int standard_app_demo_main()
{
    /* 创建任务task1 */
    user_task1_id = osal_task_create("user_task1",user_task1_entry,NULL,0x400,NULL,USER_TASK1_PRI);

    /* 创建任务task2 */
    user_task2_id = osal_task_create("user_task2",user_task2_entry,NULL,0x400,NULL,USER_TASK2_PRI);

    return 0;
}