/*----------------------------------------------------------------------------
 * Copyright (c) <2013-2015>, <Huawei Technologies Co., Ltd>
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
#include "los_err.ph"
#include "los_memory.ph"
#include "los_priqueue.ph"
#include "los_task.ph"
#if (LOSCFG_PLATFORM_EXC == YES)
#include "los_exc.h"
#endif
#include "los_hw.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#if (LOSCFG_BASE_IPC_RWLOCK == YES)

LITE_OS_SEC_BSS RWLOCK_CB_S             *g_pstAllRWLock;
LITE_OS_SEC_BSS LOS_DL_LIST             g_stUnusedRWLockList;


/*****************************************************************************
 Funtion      : osRWLockInit,
 Description  : Initializes the rwlock,
 Input        : None
 Output       : None
 Return       : LOS_OK on success ,or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT UINT32 osRWLockInit(VOID)
{
    RWLOCK_CB_S *pstRWLockNode;
    UINT32   uwIndex;

    LOS_ListInit(&g_stUnusedRWLockList);

    if (LOSCFG_BASE_IPC_RWLOCK_LIMIT == 0)   /*lint !e506*/
    {
        return LOS_ERRNO_RWLOCK_MAXNUM_ZERO;
    }

    g_pstAllRWLock = (RWLOCK_CB_S *)LOS_MemAlloc(m_aucSysMem0, (LOSCFG_BASE_IPC_RWLOCK_LIMIT * sizeof(RWLOCK_CB_S)));
    if (NULL == g_pstAllRWLock)
    {
        return LOS_ERRNO_RWLOCK_NO_MEMORY;
    }

    for (uwIndex = 0; uwIndex < LOSCFG_BASE_IPC_RWLOCK_LIMIT; uwIndex++)
    {
        pstRWLockNode                 = ((RWLOCK_CB_S *)g_pstAllRWLock) + uwIndex;
        pstRWLockNode->ucRWLockID     = uwIndex;
        pstRWLockNode->pstWOwner      = NULL;
        pstRWLockNode->ucRWLockCount  = 0;
        pstRWLockNode->ucRCount       = 0;
        pstRWLockNode->ucRWLockStat   = OS_RWLOCK_UNUSED;
        LOS_ListTailInsert(&g_stUnusedRWLockList, &pstRWLockNode->stRLockList);
    }
    return LOS_OK;
}

/*****************************************************************************
 Function     : LOS_RWLockCreate
 Description  : Create a rwlock,
 Input        : None
 Output       : puwRWLockHandle ------ RWLock operation handle
 Return       : LOS_OK on success ,or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT  UINT32  LOS_RWLockCreate (UINT32 *puwRWLockHandle)
{
    UINT32      uwIntSave;
    RWLOCK_CB_S    *pstRWLockCreated;
    LOS_DL_LIST *pstUnusedRWLock;
    UINT32      uwErrNo;
    UINT32      uwErrLine;

    if (NULL == puwRWLockHandle)
    {
        return LOS_ERRNO_RWLOCK_PTR_NULL;
    }

    uwIntSave = LOS_IntLock();
    if (LOS_ListEmpty(&g_stUnusedRWLockList))
    {
        LOS_IntRestore(uwIntSave);
        OS_GOTO_ERR_HANDLER(LOS_ERRNO_RWLOCK_ALL_BUSY);
    }

    pstUnusedRWLock                 = LOS_DL_LIST_FIRST(&(g_stUnusedRWLockList));
    LOS_ListDelete(pstUnusedRWLock);
    pstRWLockCreated                = (GET_RWLOCK_LIST(pstUnusedRWLock)); /*lint !e413*/
    LOS_ListInit(&pstRWLockCreated->stRLockList);
    LOS_ListInit(&pstRWLockCreated->stWLockList);
    pstRWLockCreated->ucRWLockCount = 0;
    pstRWLockCreated->ucRWLockStat  = OS_RWLOCK_USED;
    pstRWLockCreated->ucRCount      = 0;
    pstRWLockCreated->pstWOwner     = (LOS_TASK_CB *)NULL;
    *puwRWLockHandle                = (UINT32)pstRWLockCreated->ucRWLockID;
    LOS_IntRestore(uwIntSave);
    return LOS_OK;
ErrHandler:
    OS_RETURN_ERROR_P2(uwErrLine, uwErrNo);
}

/*****************************************************************************
 Function     : LOS_RWLockDelete
 Description  : Delete a rwlock,
 Input        : uwRWLockHandle------Mutex operation handle
 Output       : None
 Return       : LOS_OK on success ,or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT UINT32 LOS_RWLockDelete(UINT32 uwRWLockHandle)
{
    UINT32    uwIntSave;
    RWLOCK_CB_S *pstRWLockDeleted;
    UINT32   uwErrNo;
    UINT32   uwErrLine;

    if (uwRWLockHandle >= (UINT32)LOSCFG_BASE_IPC_RWLOCK_LIMIT)
    {
        OS_GOTO_ERR_HANDLER(LOS_ERRNO_RWLOCK_INVALID);
    }

    pstRWLockDeleted = GET_RWLOCK(uwRWLockHandle);
    uwIntSave = LOS_IntLock();
    if (OS_RWLOCK_UNUSED == pstRWLockDeleted->ucRWLockStat)
    {
        LOS_IntRestore(uwIntSave);
        OS_GOTO_ERR_HANDLER(LOS_ERRNO_RWLOCK_INVALID);
    }

    if ((!LOS_ListEmpty(&pstRWLockDeleted->stRLockList)) \
        || (!LOS_ListEmpty(&pstRWLockDeleted->stRLockList)) \
        || pstRWLockDeleted->ucRWLockCount)
    {
        LOS_IntRestore(uwIntSave);
        OS_GOTO_ERR_HANDLER(LOS_ERRNO_RWLOCK_PENDED);
    }

    LOS_ListAdd(&g_stUnusedRWLockList, &pstRWLockDeleted->stRLockList);
    pstRWLockDeleted->ucRWLockStat  = OS_RWLOCK_UNUSED;
    pstRWLockDeleted->pstWOwner     = NULL;
    pstRWLockDeleted->ucRWLockCount = 0;
    pstRWLockDeleted->ucRCount      = 0;

    LOS_IntRestore(uwIntSave);

    return LOS_OK;
ErrHandler:
    OS_RETURN_ERROR_P2(uwErrLine, uwErrNo);
}

/*****************************************************************************
 Function     : LOS_RLock
 Description  : Specify the rwlock r lock operation,
 Input        : uwRWLockHandle ------ RWLock operation handleone,
                uwTimeOut  ------- waiting time,
 Output       : None
 Return       : LOS_OK on success ,or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT UINT32 LOS_RLock(UINT32 uwRWLockHandle, UINT32 uwTimeout)
{
    UINT32     uwIntSave;
    RWLOCK_CB_S  *pstRWLockPended;
    UINT32     uwRetErr;
    LOS_TASK_CB  *pstRunTsk;

    if (uwRWLockHandle >= (UINT32)LOSCFG_BASE_IPC_RWLOCK_LIMIT)
    {
        OS_RETURN_ERROR(LOS_ERRNO_RWLOCK_INVALID);
    }

    pstRWLockPended = GET_RWLOCK(uwRWLockHandle);
    uwIntSave = LOS_IntLock();
    if (OS_RWLOCK_UNUSED == pstRWLockPended->ucRWLockStat)
    {
        LOS_IntRestore(uwIntSave);
        OS_RETURN_ERROR(LOS_ERRNO_RWLOCK_INVALID);
    }

    if (OS_INT_ACTIVE)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_RWLOCK_PEND_INTERR;
    }

    pstRunTsk = (LOS_TASK_CB *)g_stLosTask.pstRunTask;
    if (pstRWLockPended->ucRWLockCount == 0)
    {
        pstRWLockPended->ucRWLockCount++;
        pstRWLockPended->ucRCount++;
        LOS_IntRestore(uwIntSave);
        return LOS_OK;
    }

    if (pstRWLockPended->ucRWLockCount == pstRWLockPended->ucRCount)
    {
        pstRWLockPended->ucRWLockCount++;
        pstRWLockPended->ucRCount++;
        LOS_IntRestore(uwIntSave);
        return LOS_OK;
    }

    if (!uwTimeout)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_RWLOCK_UNAVAILABLE;
    }

    if (g_usLosTaskLock)
    {
        uwRetErr = LOS_ERRNO_RWLOCK_PEND_IN_LOCK;
        PRINT_ERR("!!!LOS_ERRNO_RWLOCK_PEND_IN_LOCK!!!\n");
#if (LOSCFG_PLATFORM_EXC == YES)
        osBackTrace();
#endif
        goto errre_uniRWLockPend;
    }

    osTaskWait(&pstRWLockPended->stRLockList, OS_TASK_STATUS_PEND, uwTimeout);
    pstRWLockPended->ucRWLockCount++;
    (VOID)LOS_IntRestore(uwIntSave);
    LOS_Schedule();

    if (pstRunTsk->usTaskStatus & OS_TASK_STATUS_TIMEOUT)
    {
        uwIntSave = LOS_IntLock();
        pstRWLockPended->ucRWLockCount--;
        pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
        (VOID)LOS_IntRestore(uwIntSave);
        uwRetErr = LOS_ERRNO_RWLOCK_TIMEOUT;
        goto error_uniRWLockPend;
    }

    return LOS_OK;

errre_uniRWLockPend:
    (VOID)LOS_IntRestore(uwIntSave);
error_uniRWLockPend:
    OS_RETURN_ERROR(uwRetErr);
}

/*****************************************************************************
 Function     : LOS_WLock
 Description  : Specify the rwlock w lock operation,
 Input        : uwRWLockHandle ------ RWLock operation handleone,
                uwTimeOut  ------- waiting time,
 Output       : None
 Return       : LOS_OK on success ,or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT UINT32 LOS_WLock(UINT32 uwRWLockHandle, UINT32 uwTimeout)
{
    UINT32     uwIntSave;
    RWLOCK_CB_S  *pstRWLockPended;
    UINT32     uwRetErr;
    LOS_TASK_CB  *pstRunTsk;

    if (uwRWLockHandle >= (UINT32)LOSCFG_BASE_IPC_RWLOCK_LIMIT)
    {
        OS_RETURN_ERROR(LOS_ERRNO_RWLOCK_INVALID);
    }

    pstRWLockPended = GET_RWLOCK(uwRWLockHandle);
    uwIntSave = LOS_IntLock();
    if (OS_RWLOCK_UNUSED == pstRWLockPended->ucRWLockStat)
    {
        LOS_IntRestore(uwIntSave);
        OS_RETURN_ERROR(LOS_ERRNO_RWLOCK_INVALID);
    }

    if (OS_INT_ACTIVE)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_RWLOCK_PEND_INTERR;
    }

    pstRunTsk = (LOS_TASK_CB *)g_stLosTask.pstRunTask;

    if (pstRWLockPended->ucRWLockCount == 0)
    {
        pstRWLockPended->ucRWLockCount++;
        pstRWLockPended->pstWOwner = pstRunTsk;
        LOS_IntRestore(uwIntSave);
        return LOS_OK;
    }

    if (pstRWLockPended->pstWOwner == pstRunTsk)
    {
        pstRWLockPended->ucRWLockCount++;
        LOS_IntRestore(uwIntSave);
        return LOS_OK;
    }

    if (!uwTimeout)
    {
        LOS_IntRestore(uwIntSave);
        return LOS_ERRNO_RWLOCK_UNAVAILABLE;
    }

    if (g_usLosTaskLock)
    {
        uwRetErr = LOS_ERRNO_RWLOCK_PEND_IN_LOCK;
        PRINT_ERR("!!!LOS_ERRNO_RWLOCK_PEND_IN_LOCK!!!\n");
#if (LOSCFG_PLATFORM_EXC == YES)
        osBackTrace();
#endif
        goto errre_uniRWLockPend;
    }

    osTaskWait(&pstRWLockPended->stWLockList, OS_TASK_STATUS_PEND, uwTimeout);
    pstRWLockPended->ucRWLockCount++;

    (VOID)LOS_IntRestore(uwIntSave);
    LOS_Schedule();

    if (pstRunTsk->usTaskStatus & OS_TASK_STATUS_TIMEOUT)
    {
        uwIntSave = LOS_IntLock();
        pstRWLockPended->ucRWLockCount--;
        pstRunTsk->usTaskStatus &= (~OS_TASK_STATUS_TIMEOUT);
        (VOID)LOS_IntRestore(uwIntSave);
        uwRetErr = LOS_ERRNO_RWLOCK_TIMEOUT;
        goto error_uniRWLockPend;
    }

    return LOS_OK;

errre_uniRWLockPend:
    (VOID)LOS_IntRestore(uwIntSave);
error_uniRWLockPend:
    OS_RETURN_ERROR(uwRetErr);
}

/*****************************************************************************
 Function     : LOS_RUnLock
 Description  : Specify the rlock unlock operation,
 Input        : uwRWLockHandle ------ RWLock operation handle,
 Output       : None
 Return       : LOS_OK on success ,or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT UINT32 LOS_RUnLock(UINT32 uwRWLockHandle)
{
    UINT32      uwIntSave;
    RWLOCK_CB_S    *pstRWLockPosted = GET_RWLOCK(uwRWLockHandle);
    LOS_TASK_CB *pstResumedTask;

    uwIntSave = LOS_IntLock();

    if ((uwRWLockHandle >= (UINT32)LOSCFG_BASE_IPC_RWLOCK_LIMIT) ||
        (OS_RWLOCK_UNUSED == pstRWLockPosted->ucRWLockStat))
    {
        LOS_IntRestore(uwIntSave);
        OS_RETURN_ERROR(LOS_ERRNO_RWLOCK_INVALID);
    }

    if ((pstRWLockPosted->ucRWLockCount == 0))
    {
        LOS_IntRestore(uwIntSave);
        OS_RETURN_ERROR(LOS_ERRNO_RWLOCK_INVALID);
    }

    if (pstRWLockPosted->ucRWLockCount > 0 && (--(pstRWLockPosted->ucRCount) > 0))
    {
        pstRWLockPosted->ucRWLockCount--;
        LOS_IntRestore(uwIntSave);
        return LOS_OK;
    }

    if ( (pstRWLockPosted->ucRCount == 0) && (!LOS_ListEmpty(&pstRWLockPosted->stWLockList)))
    {
        pstResumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&(pstRWLockPosted->stWLockList))); /*lint !e413*/
        pstRWLockPosted->ucRWLockCount--;
        pstRWLockPosted->pstWOwner     = pstResumedTask;
        osTaskWake(pstResumedTask, OS_TASK_STATUS_PEND);
        (VOID)LOS_IntRestore(uwIntSave);
        LOS_Schedule();
    }
    else
    {
        (VOID)LOS_IntRestore(uwIntSave);
    }

    return LOS_OK;
}

/*****************************************************************************
 Function     : LOS_RUnLock
 Description  : Specify the rlock unlock operation,
 Input        : uwRWLockHandle ------ RWLock operation handle,
 Output       : None
 Return       : LOS_OK on success ,or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT UINT32 LOS_WUnLock(UINT32 uwRWLockHandle)
{
    UINT32      uwIntSave;
    UINT32      count=0;
    RWLOCK_CB_S    *pstRWLockPosted = GET_RWLOCK(uwRWLockHandle);
    LOS_TASK_CB *pstResumedTask;
    LOS_TASK_CB *pstRunTsk;
    LOS_DL_LIST *item = (LOS_DL_LIST *)NULL;

    uwIntSave = LOS_IntLock();

    if ((uwRWLockHandle >= (UINT32)LOSCFG_BASE_IPC_RWLOCK_LIMIT) ||
        (OS_RWLOCK_UNUSED == pstRWLockPosted->ucRWLockStat))
    {
        LOS_IntRestore(uwIntSave);
        OS_RETURN_ERROR(LOS_ERRNO_RWLOCK_INVALID);
    }

    pstRunTsk = (LOS_TASK_CB *)g_stLosTask.pstRunTask;
    if ((pstRWLockPosted->ucRWLockCount == 0) || (pstRWLockPosted->pstWOwner != pstRunTsk))
    {
        LOS_IntRestore(uwIntSave);
        OS_RETURN_ERROR(LOS_ERRNO_RWLOCK_INVALID);
    }

    LOS_DL_LIST_FOR_EACH(item, &(pstRWLockPosted->stWLockList)) count++;
    LOS_DL_LIST_FOR_EACH(item, &(pstRWLockPosted->stRLockList)) count++;
    if((pstRWLockPosted->ucRWLockCount) > (count + 1))
    {
        pstRWLockPosted->ucRWLockCount--;
        LOS_IntRestore(uwIntSave);
        return LOS_OK;
    }

    if (!LOS_ListEmpty(&pstRWLockPosted->stWLockList))
    {
        pstResumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&(pstRWLockPosted->stWLockList))); /*lint !e413*/
        pstRWLockPosted->ucRWLockCount--;
        pstRWLockPosted->pstWOwner     = pstResumedTask;
        osTaskWake(pstResumedTask, OS_TASK_STATUS_PEND);
        (VOID)LOS_IntRestore(uwIntSave);
        LOS_Schedule();
    }
    else if(!LOS_ListEmpty(&pstRWLockPosted->stRLockList))
    {
        while(!LOS_ListEmpty(&pstRWLockPosted->stRLockList))
        {
            pstResumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&(pstRWLockPosted->stRLockList))); /*lint !e413*/
            pstRWLockPosted->ucRWLockCount--;
            pstRWLockPosted->pstWOwner = pstResumedTask;

            osTaskWake(pstResumedTask, OS_TASK_STATUS_PEND);
        }

        (VOID)LOS_IntRestore(uwIntSave);
        LOS_Schedule();
    }
    else
    {
        (VOID)LOS_IntRestore(uwIntSave);
    }

    return LOS_OK;
}

#endif /*(LOSCFG_BASE_IPC_RWLOCK == YES)*/


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
