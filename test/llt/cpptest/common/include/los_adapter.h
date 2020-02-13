/**
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>. All rights reserved.
 *
 *  DATE        AUTHOR      INSTRUCTION
 *  2020-1-7    maminjie    The first version
 */

#ifndef __LOS_ADAPTER_H__
#define __LOS_ADAPTER_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "los_typedef.h"
#include "los_task.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#ifndef SysTick_LOAD_RELOAD_Msk
#define SysTick_LOAD_RELOAD_Msk     (0xFFFFFFUL /*<< SysTick_LOAD_RELOAD_Pos*/)
#endif

/* externs */
extern uint32_t SystemCoreClock;
extern UINT8   *m_aucSysMem0;

/* interrupt */
extern UINTPTR LOS_IntLock(VOID);
extern VOID LOS_IntRestore(UINTPTR uvIntSave);
/* schedule */
extern VOID LOS_Schedule(VOID);
extern VOID LOS_StartToRun(VOID);
extern VOID osSchedule(VOID);
extern VOID osHwiInit(void);
extern UINT32 osTickStart(VOID);
extern VOID * osTskStackInit(UINT32 uwTaskID, UINT32 uwStackSize, VOID *pTopStack);
extern UINT32 osIntNumGet(VOID);

/* task */
//extern UINT32 LOS_TaskCreate(UINT32 *puwTaskID, TSK_INIT_PARAM_S *pstInitParam);
extern UINT32 LOS_TaskDelay(UINT32 uwTick);
extern UINT32 LOS_TaskDelete(UINT32 uwTaskID);
extern UINT32 LOS_TaskInfoMonitor(VOID);
/* semaphore */
extern UINT32 LOS_BinarySemCreate (UINT16 usCount, UINT32 *puwSemHandle);
extern UINT32 LOS_SemCreate (UINT16 usCount, UINT32 *puwSemHandle);
extern UINT32 LOS_SemDelete(UINT32 uwSemHandle);
extern UINT32 LOS_SemPend(UINT32 uwSemHandle, UINT32 uwTimeout);
extern UINT32 LOS_SemPost(UINT32 uwSemHandle);
/* memory */
extern void  *LOS_MemAlloc (VOID *pPool, UINT32  uwSize);
extern UINT32 LOS_MemFree(VOID *pPool, VOID *pMem);
/* mutex */
extern UINT32 LOS_MuxCreate (UINT32 *puwMuxHandle);
extern UINT32 LOS_MuxDelete(UINT32 uwMuxHandle);
extern UINT32 LOS_MuxPend(UINT32 uwMuxHandle, UINT32 uwTimeout);
extern UINT32 LOS_MuxPost(UINT32 uwMuxHandle);
/* queue */
extern UINT32 LOS_QueueCreate(CHAR *pcQueueName, UINT16 usLen, UINT32 *puwQueueID,
                                    UINT32 uwFlags,UINT16 usMaxMsgSize );
extern UINT32 LOS_QueueDelete(UINT32 uwQueueID);
extern UINT32 LOS_QueueReadCopy(UINT32  uwQueueID, VOID *  pBufferAddr,
                                    UINT32 * puwBufferSize, UINT32  uwTimeOut);
extern UINT32 LOS_QueueWriteCopy( UINT32 uwQueueID, VOID * pBufferAddr,
                                    UINT32 uwBufferSize, UINT32 uwTimeOut );
/* tick */
extern UINT64 LOS_TickCountGet(VOID);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __LOS_ADAPTER_H__ */
