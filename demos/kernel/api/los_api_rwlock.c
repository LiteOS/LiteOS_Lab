/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
 /*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "los_rwlock.h"
#include "los_task.h"
#include "los_api_rwlock.h"
#include "los_inspect_entry.h"

#ifdef LOSCFG_LIB_LIBC
#include "string.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/* rwlock handle id */
static UINT32 g_TestRWLock01;

/* task pid */
static UINT32 g_TestTaskID01;
static UINT32 g_TestTaskID02;
static UINT32 g_TestTaskID03;
static UINT32 g_TestTaskID04;


static VOID Example_RWLockTask1(VOID)
{
    UINT32 uwRet = LOS_OK;
    UINT32 count =20;

    dprintf("task1 try to get rwlock, wait 50 Tick. \r\n");
    /* get rwlock */
    uwRet = LOS_RWReadLock(g_TestRWLock01, 50);

    if (uwRet == LOS_OK)
    {
        dprintf("task1 get rwlock g_TestRWLock01. \r\n");
    }
    else if (uwRet == LOS_ERRNO_RWLOCK_TIMEOUT)
    {
        dprintf("task1 timeout and try to get  rwlock, wait forever. \r\n");
        /* LOS_WAIT_FOREVER type get rwlock, LOS_RWWriteLock return until has been get rwlock */
        uwRet = LOS_RWReadLock(g_TestRWLock01, LOS_WAIT_FOREVER);
        if (uwRet == LOS_OK)
        {
            dprintf("task1 wait forever,got g_TestRWLock01 success. \r\n");
        }
    }
    while(count--)
    {
        dprintf("Task 1 r\r\n");
        LOS_TaskDelay(10);
    }
    dprintf("task1 unlock the wlock  \r\n");
    LOS_RWReadUnLock(g_TestRWLock01);
    return;
}

static VOID Example_RWLockTask2(VOID)
{
    UINT32 uwRet = LOS_OK;
    UINT32 count=10;

    dprintf("task2 try to get rlock, wait forever. \r\n");
    /* get rwlock */
    uwRet = LOS_RWReadLock(g_TestRWLock01, LOS_WAIT_FOREVER);
    if(uwRet != LOS_OK)
    {
        dprintf("task2 LOS_RWWriteLock failed . \r\n");
        return;
    }

    dprintf("task2 get  g_TestRWLock01.  \r\n");

    /* task delay 20 ticks */
    LOS_TaskDelay(20);

    while(count--)
    {
        dprintf("Task 2 r\r\n");
        LOS_TaskDelay(10);
    }

    dprintf("task2 unlock the rlock \r\n");
    /* release rwlock */
    LOS_RWReadUnLock(g_TestRWLock01);
    return;
}

static VOID Example_RWLockTask3(VOID)
{
    UINT32 uwRet;
    UINT32 count=30;

    dprintf("task3 try to get rwlock. \r\n");
    /* get rwlock */
    uwRet = LOS_RWWriteLock(g_TestRWLock01, LOS_WAIT_FOREVER);
    if(uwRet != LOS_OK)
    {
        dprintf("task3 LOS_RWWriteLock failed . \r\n");
        return;
    }

    dprintf("task3 get rwlock g_TestRWLock01 . \r\n");

    /* task delay 20 ticks */
    LOS_TaskDelay(20);

    while(count--)
    {
        dprintf("Task 3 w\r\n");
        LOS_TaskDelay(10);
    }
    dprintf("task3 resumed and post the g_TestRWLock01 \r\n");
    LOS_RWWriteUnLock(g_TestRWLock01);
    return;
}

static VOID Example_RWLockTask4(VOID)
{
    UINT32 uwRet;
    UINT32 count=30;

    dprintf("task4 try to get rwlock. \r\n");
    /* get rwlock */
    uwRet = LOS_RWReadLock(g_TestRWLock01, LOS_WAIT_FOREVER);
    if(uwRet != LOS_OK)
    {
        dprintf("task4 LOS_RWReadLock failed . \r\n");
        return;
    }

    dprintf("task4 get rwlock g_TestRWLock01 . \r\n");

    /* task delay 20 ticks */
    LOS_TaskDelay(20);

    while(count--)
    {
        dprintf("Task 4 w\r\n");
        LOS_TaskDelay(10);
    }
    dprintf("task4 unlock the rlock \r\n");
    LOS_RWReadUnLock(g_TestRWLock01);
    return;
}


UINT32 Example_RWLock(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTask1;
    TSK_INIT_PARAM_S stTask2;
    TSK_INIT_PARAM_S stTask3;
    TSK_INIT_PARAM_S stTask4;

    /* create rwlock */
    LOS_RWLockCreate(&g_TestRWLock01);

    /* lock task schedue */
    LOS_TaskLock();

    /* create task1 */
    memset(&stTask1, 0, sizeof(TSK_INIT_PARAM_S));
    stTask1.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_RWLockTask1;
    stTask1.pcName       = "RWLockTsk1";
    stTask1.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTask1.usTaskPrio   = 5;
    uwRet = LOS_TaskCreate(&g_TestTaskID01, &stTask1);
    if (uwRet != LOS_OK)
    {
        dprintf("task1 create failed . \r\n");
        return LOS_NOK;
    }

    /* create task2 */
    memset(&stTask2, 0, sizeof(TSK_INIT_PARAM_S));
    stTask2.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_RWLockTask2;
    stTask2.pcName       = "RWLockTsk2";
    stTask2.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTask2.usTaskPrio   = 4;
    uwRet = LOS_TaskCreate(&g_TestTaskID02, &stTask2);
    if (uwRet != LOS_OK)
    {
        dprintf("task2 create failed . \r\n");
        return LOS_NOK;
    }

    /* create task3 */
    memset(&stTask3, 0, sizeof(TSK_INIT_PARAM_S));
    stTask3.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_RWLockTask3;
    stTask3.pcName       = "RWLockTsk3";
    stTask3.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTask3.usTaskPrio   = 6;
    uwRet = LOS_TaskCreate(&g_TestTaskID03, &stTask3);
    if (uwRet != LOS_OK)
    {
        dprintf("task3 create failed . \r\n");
        return LOS_NOK;
    }

    /* create task4 */
    memset(&stTask4, 0, sizeof(TSK_INIT_PARAM_S));
    stTask4.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_RWLockTask4;
    stTask4.pcName       = "RWLockTsk4";
    stTask4.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTask4.usTaskPrio   = 6;
    uwRet = LOS_TaskCreate(&g_TestTaskID04, &stTask4);
    if (uwRet != LOS_OK)
    {
        dprintf("task4 create failed . \r\n");
        return LOS_NOK;
    }

    /* unlock task schedue */
    LOS_TaskUnlock();
    /* task delay 300 ticks */
    LOS_TaskDelay(300);

    /* delete rwlock */
    LOS_RWLockDelete(g_TestRWLock01);
    return LOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
