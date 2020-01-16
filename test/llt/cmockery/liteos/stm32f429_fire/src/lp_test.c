/**
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>. All rights reserved.
 *
 *  DATE        AUTHOR      INSTRUCTION
 *  2020-1-9    maminjie    The first version
 */

#ifdef CONFIG_LP

#include "osal.h"
#include "los_swtmr.h"
#include "los_hwi.h"

#if (LOSCFG_PLATFORM_HWI == NO)
UINT32 LOS_HwiCreate(HWI_HANDLE_T uwHwiNum,
                     HWI_PRIOR_T usHwiPrio,
                     HWI_MODE_T usMode,
                     HWI_PROC_FUNC pfnHandler,
                     HWI_ARG_T uwArg)
{
    return LOS_OK;
}
#endif

static int task1(void *args)
{
    for (;;)
    {
        osal_task_sleep(5000);
        printf("l'm task1, sleep 5000ms\n");
    }
    return 0;
}

static int task2(void *args)
{
    for (;;)
    {
        osal_task_sleep(7000);
        printf("l'm task2, sleep 7000ms\n");
    }
    return 0;
}

static UINT32 g_timercount1 = 0;
static UINT32 g_timercount2 = 0;

static VOID timer1_callback(UINT32 arg)
{
    UINT32 tick_last1;

    g_timercount1++;
    tick_last1 = (UINT32)LOS_TickCountGet();
    printf("l'm timer1, g_timercount1=%d ,tick_last1=%d .\n", g_timercount1, tick_last1);
}

static VOID timer2_callback(UINT32 arg)
{
    UINT32 tick_last2;

    g_timercount2++;
    tick_last2 = (UINT32)LOS_TickCountGet();
    printf("l'm timer2, g_timercount2=%d ,tick_last2=%d .\n", g_timercount2, tick_last2);
}

static int lp_main(void)
{
    UINT16 id1;
    UINT16 id2;// timer id

    osal_init();

    osal_task_create("task1", task1, 0, 0x800, NULL, 10);
    osal_task_create("task2", task2, 0, 0x800, NULL, 11);

#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == YES)
    LOS_SwtmrCreate(4000, LOS_SWTMR_MODE_PERIOD, timer1_callback, &id1, 1, OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_SENSITIVE);
    LOS_SwtmrCreate(6000, LOS_SWTMR_MODE_PERIOD, timer2_callback, &id2, 1, OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_SENSITIVE);
#else
    LOS_SwtmrCreate(4000, LOS_SWTMR_MODE_PERIOD, timer1_callback, &id1, 1);
    LOS_SwtmrCreate(6000, LOS_SWTMR_MODE_PERIOD, timer2_callback, &id2, 1);
#endif
    LOS_SwtmrStart(id1);
    LOS_SwtmrStart(id2);

    for (;;)
    {
        osal_task_sleep(12000);
        printf("l'm lp_main, sleep 12000ms\n");
    }
    return 0;
}

int lp_test(void)
{
    int ret = -1;
    UINT32 uwRet = LOS_OK;
    UINT32 handle;
    TSK_INIT_PARAM_S task_init_param;

    memset (&task_init_param, 0, sizeof (TSK_INIT_PARAM_S));
    task_init_param.uwArg = (unsigned int)NULL;
    task_init_param.usTaskPrio = 5;
    task_init_param.pcName =(char *) "lp_main";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)lp_main;
    task_init_param.uwStackSize = 0x1000;
    uwRet = LOS_TaskCreate(&handle, &task_init_param);
    if (LOS_OK == uwRet){
        ret = 0;
    }
    return ret;
}

#endif
