/* 使用osal接口需要包含该头文件 */
#include <osal.h>

/* 任务优先级宏定义（shell任务的优先级为10） */
#define USER_TASK1_PRI  12  //低优先级
#define USER_TASK2_PRI  11  //高优先级

/* 共享资源 */
uint32_t public_value = 0;

/* 互斥锁索引ID */
osal_mutex_t public_value_mutex;

/* 任务task1入口函数 */
static int user_task1_entry()
{
    while(1)
    {
        /* 尝试获取互斥锁 */
        if(true == osal_mutex_lock(public_value_mutex))
        {
            /* 获取到互斥锁，对共享资源进行操作 */
            printf("\r\ntask1: lock a mutex.\r\n");
            public_value += 10;
            printf("task1: public_value = %ld.\r\n", public_value);

            /* 对共享资源操作完毕，释放互斥锁 */
            printf("task1: unlock a mutex.\r\n\r\n");
            osal_mutex_unlock(public_value_mutex);

            /* 满足条件则结束任务 */
            if(public_value > 100)
                break;

        }
    }

    /* while(1)会执行结束，所以需要返回值 */
    return 0;
}

/* 任务task2入口函数 */
static int user_task2_entry()
{
    while (1)
    {
        /* 尝试获取互斥锁 */
       if(true == osal_mutex_lock(public_value_mutex))
        {
            /* 获取到互斥锁，对共享资源进行操作 */
            printf("\r\ntask2: lock a mutex.\r\n");
            public_value += 5; 
            printf("task2: public_value = %ld.\r\n", public_value);

            /* 对共享资源操作完毕，释放互斥锁 */
            printf("task2: unlock a mutex.\r\n\r\n");
            osal_mutex_unlock(public_value_mutex);

            /* 满足条件则结束任务 */
            if(public_value > 90)
                break;
            /* 优先级较高，需要挂起一下，让task1获取到互斥锁，否则task2再次上锁，形成死锁 */
            osal_task_sleep(10);
        }
    }

    /* while(1)会执行结束，所以需要返回值 */
    return 0;
}

/* 标准demo启动函数，函数名不要修改，否则会影响下一步实验 */
int standard_app_demo_main()
{
    /* 创建互斥锁public_value_mutex */
    osal_mutex_create(&public_value_mutex);

    /* 创建任务task1 */
    osal_task_create("user_task1",user_task1_entry,NULL,0x400,NULL,USER_TASK1_PRI);

    /* 创建任务task2 */
    osal_task_create("user_task2",user_task2_entry,NULL,0x400,NULL,USER_TASK2_PRI);

    return 0;
}