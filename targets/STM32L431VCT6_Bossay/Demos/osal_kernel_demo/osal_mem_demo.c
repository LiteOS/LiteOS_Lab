/* 使用osal接口需要包含该头文件 */
#include <osal.h>

/* 任务入口函数 */
static int mem_access_task_entry()
{
    uint32_t i = 0;     //循环变量
    size_t mem_size;    //申请的内存块大小
    uint8_t* mem_ptr = NULL;    //内存块指针

    while (1)
    {
        /* 每次循环将申请内存的大小扩大一倍 */
        mem_size = 1 << i++;

        /* 尝试申请分配内存 */
        mem_ptr = osal_malloc(mem_size);

        /* 判断是否申请成功 */
        if(mem_ptr != NULL)
        {
            /* 申请成功，打印信息 */
            printf("access %d bytes memory success!\r\n", mem_size);

            /* 释放申请的内存，便于下次申请 */
            osal_free(mem_ptr);

            /* 将内存块指针置为NULL，避免称为野指针 */
            mem_ptr = NULL;

            printf("free memory success!\r\n");

        }
        else
        {
            /* 申请失败，打印信息，任务结束 */
            printf("access %d bytes memory failed!\r\n", mem_size);
            return 0;
        }
    }
}

/* 标准demo启动函数，函数名不要修改，否则会影响下一步实验 */
int standard_app_demo_main()
{
    /* 创建任务，任务优先级为11，shell任务的优先级为10 */
    osal_task_create("mem_access_task",mem_access_task_entry,NULL,0x400,NULL,11);
    return 0;
}