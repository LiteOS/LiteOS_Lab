/* 使用osal接口需要包含该头文件 */
#include <osal.h>

/* 任务优先级宏定义（shell任务的优先级为10） */
#define USER_TASK1_PRI  12  //低优先级
#define USER_TASK2_PRI  11  //高优先级

/* 信号量索引ID */
osal_semp_t sync_semp;

/* 任务task1入口函数 */
static int user_task1_entry()
{
    while(1)
    {
        /* 在串口打印信息 */
        printf("task 1 post a semp!\r\n");

        /* 打印完毕释放信号量 */
        osal_semp_post(sync_semp);

        /* 任务主动挂起2s */
        osal_task_sleep(2*1000);
    }
    return 0;
}

/* 任务task2入口函数 */
static int user_task2_entry()
{
    while (1)
    {
        /* 优先级高，抢占执行打印信息 */
        printf("task2 is waiting for a semp...\r\n");

        /* 申请信号量，申请失败则挂起等待 */
        osal_semp_pend(sync_semp, cn_osal_timeout_forever);

        /* 一旦申请到信号量，则恢复执行 */
        printf("task 2 access a semp!\r\n");
    }
    return 0;
}

/* 标准demo启动函数，函数名不要修改，否则会影响下一步实验 */
int standard_app_demo_main()
{
    /* 创建信号量sync_semp */
    osal_semp_create(&sync_semp, 1, 0);
    printf("sync_semp semp create success.\r\n");

    /* 创建任务task1 */
    osal_task_create("user_task1",user_task1_entry,NULL,0x400,NULL,USER_TASK1_PRI);

    /* 创建任务task2 */
    osal_task_create("user_task2",user_task2_entry,NULL,0x400,NULL,USER_TASK2_PRI);

    return 0;
}