/**
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>. All rights reserved.
 *
 *  DATE        AUTHOR      INSTRUCTION
 *  2020-1-7    maminjie    The first version
 */

#include "los_adapter.h"
#include "mem.h"

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

uint32_t SystemCoreClock = 16000000;

UINT32   g_semCrt = -1;
UINT32   g_vuwIntCount = 0;
UINT8   *m_aucSysMem0 = NULL;

static UINT8 s_sysMem0[OS_SYS_MEM_SIZE];
static UINT8 s_sysMem1[OS_SYS_MEM_SIZE];

// UINT32 __LOS_HEAP_ADDR_START__;
// UINT32 __LOS_HEAP_ADDR_END__;

const struct phys_mem system_phys_mem [] =
{
    { (unsigned long)s_sysMem0, (unsigned long)s_sysMem0 + OS_SYS_MEM_SIZE },
    { (unsigned long)s_sysMem1, (unsigned long)s_sysMem1 + OS_SYS_MEM_SIZE },
    { 0, 0 }
};

/************************************* interrupt *****************************/
UINTPTR LOS_IntLock(VOID)
{
    return 0;
}
VOID LOS_IntRestore(UINTPTR uvIntSave)
{
    return;
}

/************************************* schedule ******************************/
VOID LOS_Schedule(VOID)
{
    return;
}
VOID LOS_StartToRun(VOID)
{
    return;
}
VOID osSchedule(VOID)
{

}
VOID osHwiInit(void)
{

}
UINT32 osTickStart(VOID)
{
    return LOS_OK;
}
VOID * osTskStackInit(UINT32 uwTaskID, UINT32 uwStackSize, VOID *pTopStack)
{
    return NULL;
}
UINT32 osIntNumGet(VOID)
{
    return 20;
}

/************************************* task ***********************************/
typedef void * (*thread_func)(void *);
UINT32 LOS_TaskCreate(UINT32 *puwTaskID, TSK_INIT_PARAM_S *pstInitParam)
{
    return 0;
}

UINT32 LOS_TaskDelay(UINT32 uwTick)
{
    return 0;
}

UINT32 LOS_TaskDelete(UINT32 uwTaskID)
{
    return 0;
}

UINT32 LOS_TaskInfoMonitor(VOID)
{
    return 0;
}

/************************************* semaphore *******************************/
UINT32 LOS_BinarySemCreate (UINT16 usCount, UINT32 *puwSemHandle)
{
    return g_semCrt;
}

UINT32 LOS_SemCreate (UINT16 usCount, UINT32 *puwSemHandle)
{
    return 0;
}

UINT32 LOS_SemDelete(UINT32 uwSemHandle)
{
    return 0;
}
UINT32 LOS_SemPend(UINT32 uwSemHandle, UINT32 uwTimeout)
{
    usleep(1000);
    return 0;
}
UINT32 LOS_SemPost(UINT32 uwSemHandle)
{
    return 0;
}

/************************************* mutex ***********************************/
UINT32 LOS_MuxPend(UINT32 uwMuxHandle, UINT32 uwTimeout)
{
    return 0;
}

UINT32 LOS_MuxPost(UINT32 uwMuxHandle)
{
    return 0;
}

UINT32  LOS_MuxCreate (UINT32 *puwMuxHandle)
{
    return 0;
}

UINT32 LOS_MuxDelete(UINT32 uwMuxHandle)
{
    return 0;
}

/************************************* mutex ***********************************/
UINT32 LOS_QueueCreate(CHAR *pcQueueName, UINT16 usLen, UINT32 *puwQueueID,
                            UINT32 uwFlags,UINT16 usMaxMsgSize )
{
    return 0;
}

UINT32 LOS_QueueReadCopy(UINT32  uwQueueID,VOID *  pBufferAddr,
                            UINT32 * puwBufferSize, UINT32  uwTimeOut)
{
    return 0;
}

UINT32 LOS_QueueDelete(UINT32 uwQueueID)
{
    return 0;
}

UINT32 LOS_QueueWriteCopy( UINT32 uwQueueID,VOID * pBufferAddr,
                             UINT32 uwBufferSize, UINT32 uwTimeOut )
{
    return 0;
}

/************************************* tick ***********************************/
UINT64 LOS_TickCountGet (VOID)
{
    return 0;
}
