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

#include "los_hwi.h"
#include "gd32vf103_eclic.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*lint -save -e40 -e522 -e533*/

extern void LOS_TickHandler(void);
extern void SysTick_Handler(void);
/*lint -restore*/
UINT32  g_vuwIntCount = 0;
#ifdef LOS_LOCATION_VECTOR_IAR
#pragma  location = ".vector"
#endif

HWI_HANDLER_T m_pstHwiSlaveForm[OS_RV_IRQ_VECTOR_CNT] = {0};

/*****************************************************************************
 Function    : osIntNumGet
 Description : Get a interrupt number
 Input       : None
 Output      : None
 Return      : Interrupt Indexes number
 *****************************************************************************/
LITE_OS_SEC_TEXT_MINOR UINT32 osIntNumGet(VOID)
{
    UINT32 uwIntNum;

    uwIntNum = LOS_IntNumGet();
    return uwIntNum;
}

/*****************************************************************************
 Function    : osHwiDefaultHandler
 Description : default handler of the hardware interrupt
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
LITE_OS_SEC_TEXT_MINOR VOID  osHwiDefaultHandler(VOID)
{
    UINT32 irq_num = osIntNumGet();
    while(1);
}

/*****************************************************************************
 Function    : osInterrupt
 Description : Hardware interrupt entry function
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
LITE_OS_SEC_TEXT VOID  osInterrupt(VOID)
{
    UINT32 uwHwiIndex;
    UINT32 uwIntSave;

    uwIntSave = LOS_IntLock();
    g_vuwIntCount++;
    LOS_IntRestore(uwIntSave);

    uwHwiIndex = osIntNumGet();

    if (m_pstHwiSlaveForm[uwHwiIndex - OS_RV_SYS_VECTOR_CNT].pfnHandler != NULL)
    {
#if (OS_HWI_WITH_ARG == YES)
        m_pstHwiSlaveForm[uwHwiIndex- OS_RV_SYS_VECTOR_CNT].pfnHandler(m_pstHwiSlaveForm[uwHwiIndex- OS_RV_SYS_VECTOR_CNT].pParm);
#else
        m_pstHwiSlaveForm[uwHwiIndex- OS_RV_SYS_VECTOR_CNT].pfnHandler();
#endif
    }
	else
    {
        osHwiDefaultHandler();
    }

    uwIntSave = LOS_IntLock();
    g_vuwIntCount--;
    LOS_IntRestore(uwIntSave);
}


/*****************************************************************************
 Function    : osHwiInit
 Description : initialization of the hardware interrupt
 Input       : None
 Output      : None
 Return      : OS_SUCCESS
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT VOID osHwiInit()
{
    UINT32 uwIndex;
    for(uwIndex = OS_RV_SYS_VECTOR_CNT; uwIndex < OS_RV_VECTOR_CNT; uwIndex++)
    {
        m_pstHwiSlaveForm[uwIndex - OS_RV_SYS_VECTOR_CNT].pfnHandler = osHwiDefaultHandler;
    }
}

/*****************************************************************************
 Function    : LOS_HwiCreate
 Description : create hardware interrupt
 Input       : uwHwiNum   --- hwi num to create
               usHwiPrio  --- priority of the hwi
               usMode     --- unused
               pfnHandler --- hwi handler
               uwArg      --- param of the hwi handler
 Output      : None
 Return      : OS_SUCCESS on success or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT UINT32 LOS_HwiCreate( HWI_HANDLE_T  uwHwiNum,
                                      HWI_PRIOR_T   usHwiPrio,
                                      HWI_MODE_T    usMode,
                                      HWI_PROC_FUNC pfnHandler,
                                      HWI_ARG_T     uwArg )
{
    UINTPTR uvIntSave;

    if (NULL == pfnHandler)
    {
        return OS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    if (uwHwiNum >= OS_RV_IRQ_VECTOR_CNT)
    {
        return OS_ERRNO_HWI_NUM_INVALID;
    }
    if (usHwiPrio > OS_HWI_PRIO_LOWEST)
    {
        return OS_ERRNO_HWI_PRIO_INVALID;
    }

    uvIntSave = LOS_IntLock();

    osSetVector(uwHwiNum, pfnHandler);

#if (OS_HWI_WITH_ARG == YES)
     m_pstHwiSlaveForm[uwHwiNum - OS_RV_SYS_VECTOR_CNT].pParm = (VOID*)uwArg;
#endif

    eclic_irq_enable(uwHwiNum, 1, usHwiPrio);

    LOS_IntRestore(uvIntSave);

    return LOS_OK;

}

/*****************************************************************************
 Function    : LOS_HwiDelete
 Description : Delete hardware interrupt
 Input       : uwHwiNum   --- hwi num to delete
 Output      : None
 Return      : LOS_OK on success or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT UINT32 LOS_HwiDelete(HWI_HANDLE_T uwHwiNum)
{
    UINT32 uwIntSave;

    if (uwHwiNum >= OS_RV_IRQ_VECTOR_CNT)
    {
        return OS_ERRNO_HWI_NUM_INVALID;
    }

    eclic_irq_disable(uwHwiNum);

    uwIntSave = LOS_IntLock();

    m_pstHwiSlaveForm[uwHwiNum - OS_RV_SYS_VECTOR_CNT].pfnHandler = (HWI_PROC_FUNC)osHwiDefaultHandler;

    LOS_IntRestore(uwIntSave);

    return LOS_OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


