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

 /**@defgroup los_hwi Hardware interrupt
   *@ingroup kernel
 */
#ifndef _LOS_HWI_H
#define _LOS_HWI_H

#include "los_base.h"
#include "los_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @ingroup los_hwi
 * Define the type of a hardware interrupt number.
 */
typedef UINT32                                              HWI_HANDLE_T;

/**
 * @ingroup los_hwi
 * Define the type of a hardware interrupt priority.
 */
typedef UINT16                                              HWI_PRIOR_T;

/**
 * @ingroup los_hwi
 * Define the type of hardware interrupt mode configurations.
 */
typedef UINT16                                              HWI_MODE_T;

/**
 * @ingroup los_hwi
 * Define the type of the parameter used for the hardware interrupt creation function. The function of this parameter varies among platforms.
 */
typedef UINT32                                              HWI_ARG_T;

/**
 * @ingroup  los_hwi
 * Define the type of a hardware interrupt handling function.
 */
typedef VOID (* HWI_PROC_FUNC)(void);

/**
 * @ingroup  los_hwi
 * Define the type of a hardware interrupt vector table function.
 */
typedef VOID (**HWI_VECTOR_FUNC)(void);


/**
 * @ingroup los_hwi
 * Count of interrupts.
 */
extern UINT32  g_vuwIntCount;

/**
 * @ingroup los_hwi
 * An interrupt is active.
 */
#define OS_INT_ACTIVE               (g_vuwIntCount > 0)

/**
 * @ingroup los_hwi
 * An interrupt is inactive.
 */
#define OS_INT_INACTIVE             (!(OS_INT_ACTIVE))

/**
 * @ingroup los_hwi
 * Highest priority of a hardware interrupt.
 */
#define  OS_HWI_PRIO_HIGHEST        0

/**
 * @ingroup los_hwi
 * Lowest priority of a hardware interrupt.
 */
#define  OS_HWI_PRIO_LOWEST         7


/**
 * @ingroup los_hwi
 * Maximum interrupt number.
 */
#define OS_HWI_MAX                  86

/**
 * @ingroup los_hwi
 * Minimum interrupt number.
 */
#define OS_HWI_MIN                  19

/**
 * @ingroup los_hwi
 * Count of M4 system interrupt vector.
 */
#define OS_RV_SYS_VECTOR_CNT        19

/**
 * @ingroup los_hwi
 * Count of M4 IRQ interrupt vector.
 */
#define OS_RV_IRQ_VECTOR_CNT        68

/**
 * @ingroup los_hwi
 * Count of M4 interrupt vector.
 */
#define OS_RV_VECTOR_CNT            (OS_RV_SYS_VECTOR_CNT + OS_RV_IRQ_VECTOR_CNT)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Invalid interrupt number.
 *
 * Value: 0x02000900
 *
 * Solution: Ensure that the interrupt number is valid. The value range of the interrupt number applicable for a Cortex-M4 platform is [0,240].
 */
#define OS_ERRNO_HWI_NUM_INVALID                            LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x00)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Null hardware interrupt handling function.
 *
 * Value: 0x02000901
 *
 * Solution: Pass in a valid non-null hardware interrupt handling function.
 */
#define OS_ERRNO_HWI_PROC_FUNC_NULL                         LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x01)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Insufficient interrupt resources for hardware interrupt creation.
 *
 * Value: 0x02000902
 *
 * Solution: Increase the configured maximum number of supported hardware interrupts.
 */
#define OS_ERRNO_HWI_CB_UNAVAILABLE                         LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x02)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Insufficient memory for hardware interrupt initialization.
 *
 * Value: 0x02000903
 *
 * Solution: Expand the configured memory.
 */
#define OS_ERRNO_HWI_NO_MEMORY                              LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x03)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: The interrupt has already been created.
 *
 * Value: 0x02000904
 *
 * Solution: Check whether the interrupt specified by the passed-in interrupt number has already been created.
 */
#define OS_ERRNO_HWI_ALREADY_CREATED                        LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x04)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Invalid interrupt priority.
 *
 * Value: 0x02000905
 *
 * Solution: Ensure that the interrupt priority is valid. The value range of the interrupt priority applicable for a Cortex-M4 platform is [OS_HWI_PRIO_HIGHEST,OS_HWI_PRIO_LOWEST].
 */
#define OS_ERRNO_HWI_PRIO_INVALID                           LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x05)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Incorrect interrupt creation mode.
 *
 * Value: 0x02000906
 *
 * Solution: The interrupt creation mode can be only set to OS_HWI_MODE_COMM or OS_HWI_MODE_FAST of which the value can be 0 or 1.
 */
#define OS_ERRNO_HWI_MODE_INVALID                           LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x06)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: The interrupt has already been created as a fast interrupt.
 *
 * Value: 0x02000907
 *
 * Solution: Check whether the interrupt specified by the passed-in interrupt number has already been created.
 */
#define OS_ERRNO_HWI_FASTMODE_ALREADY_CREATED               LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x07)






#if (OS_HWI_WITH_ARG == YES)
typedef VOID (* HWI_PROC_FUNC)(VOID *pParm);
#else
typedef VOID (* HWI_PROC_FUNC)(VOID);
#endif

typedef struct
{
    HWI_PROC_FUNC pfnHandler;
#if (OS_HWI_WITH_ARG == YES)
    VOID*         pParm;
#endif
} HWI_HANDLER_T;



/**
 * @ingroup los_hwi
 * hardware interrupt Slave form mapping handling function array.
 */
extern HWI_HANDLER_T m_pstHwiSlaveForm[OS_RV_IRQ_VECTOR_CNT];

extern VOID Reset_Handler(VOID);

#ifndef OS_HWI_WITH_ARG
#define OS_HWI_WITH_ARG             NO
#endif




/**
 * @ingroup los_hwi
 * Set interrupt vector table.
 */
#define osSetVector(uwNum, pfnVector)       \
    m_pstHwiSlaveForm[uwNum - OS_RV_SYS_VECTOR_CNT].pfnHandler = pfnVector;


/**
 * @ingroup  los_hwi
 * @brief Create a hardware interrupt.
 *
 * @par Description:
 * This API is used to configure a hardware interrupt and register a hardware interrupt handling function.
 *
 * @attention
 * <ul>
 * <li>The hardware interrupt module is usable only when the configuration item for hardware interrupt tailoring is enabled.</li>
 * <li>Hardware interrupt number value range: [OS_HWI_MIN,OS_HWI_MAX]. The value range applicable for a Cortex-M4 platform is [0,240].</li>
 * <li>OS_HWI_MAX_NUM specifies the maximum number of interrupts that can be created.</li>
 * <li>Before executing an interrupt on a platform, refer to the chip manual of the platform.</li>
 * </ul>
 *
 * @param  uwHwiNum   [IN] Type#HWI_HANDLE_T: hardware interrupt number. The value range applicable for a Cortex-M4 platform is [0,240].
 * @param  usHwiPrio  [IN] Type#HWI_PRIOR_T: hardware interrupt priority. Ignore this parameter temporarily.
 * @param  usMode     [IN] Type#HWI_MODE_T: hardware interrupt mode. Ignore this parameter temporarily.
 * @param  pfnHandler [IN] Type#HWI_PROC_FUNC: interrupt handler used when a hardware interrupt is triggered.
 * @param  uwArg      [IN] Type#HWI_ARG_T: input parameter of the interrupt handler used when a hardware interrupt is triggered.
 *
 * @retval #OS_ERRNO_HWI_PROC_FUNC_NULL               0x02000901: Null hardware interrupt handling function.
 * @retval #OS_ERRNO_HWI_NUM_INVALID                     0x02000900: Invalid interrupt number.
 * @retval #OS_ERRNO_HWI_NO_MEMORY                     0x02000903: Insufficient memory for hardware interrupt creation.
 * @retval #OS_ERRNO_HWI_ALREADY_CREATED              0x02000904: The interrupt handler being created has already been created.
 * @retval #LOS_OK                                                       0,               : The interrupt is successfully created.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_HwiCreate( HWI_HANDLE_T  uwHwiNum,
                           HWI_PRIOR_T   usHwiPrio,
                           HWI_MODE_T    usMode,
                           HWI_PROC_FUNC pfnHandler,
                           HWI_ARG_T     uwArg
                           );


/**
 * @ingroup  los_hwi
 * @brief: Hardware interrupt entry function.
 *
 * @par Description:
 * This API is used as all hardware interrupt handling function entry.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param:None.
 *
 * @retval:None.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern VOID  osInterrupt(VOID);



/**
 * @ingroup  los_hwi
 * @brief: Get a interrupt number.
 *
 * @par Description:
 * This API is used to get the current interrupt number.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param: None.
 *
 * @retval: Interrupt Indexes number.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 osIntNumGet(VOID);



/**
 * @ingroup  los_hwi
 * @brief: Reset the vector table.
 *
 * @par Description:
 * This API is used to reset the vector table.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param:None.
 *
 * @retval:None.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern VOID  osResetVector(VOID);



/**
 * @ingroup  los_hwi
 * @brief: Default vector handling function.
 *
 * @par Description:
 * This API is used to configure interrupt for null function.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param:None.
 *
 * @retval:None.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern VOID  osHwiDefaultHandler(VOID);



/**
 * @ingroup  los_hwi
 * @brief: Pended System Call.
 *
 * @par Description:
 * PendSV can be pended and is useful for an OS to pend an exception
 * so that an action can be performed after other important tasks are completed.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param:None.
 *
 * @retval:None.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern VOID  PendSV_Handler(VOID);

 /**
 *@ingroup los_hwi
 *@brief Enable all interrupts.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to enable all IRQ and FIQ interrupts in the CPSR.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *
 *@param None.
 *
 *@retval CPSR value obtained after all interrupts are enabled.
 *@par Dependency:
 *<ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_IntRestore
 *@since Huawei LiteOS V100R001C00
 */
extern UINTPTR LOS_IntUnLock(VOID);



 /**
 *@ingroup los_hwi
 *@brief Disable all interrupts.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to disable all IRQ and FIQ interrupts in the CPSR.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *
 *@param None.
 *
 *@retval CPSR value obtained before all interrupts are disabled.
 *@par Dependency:
 *<ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_IntRestore
 *@since Huawei LiteOS V100R001C00
 */
extern UINTPTR LOS_IntLock(VOID);



 /**
 *@ingroup los_hwi
 *@brief Restore interrupts.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to restore the CPSR value obtained before all interrupts are disabled.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>This API can be called only after all interrupts are disabled, and the input parameter value should be the value returned by calling the all interrupt disabling API.</li>
 *</ul>
 *
 *@param uvIntSave [IN] CPSR value obtained before all interrupts are disabled.
 *
 *@retval None.
 *@par Dependency:
 *<ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_IntLock
 *@since Huawei LiteOS V100R001C00
 */
extern VOID LOS_IntRestore(UINTPTR uvIntSave);



/**
 * @ingroup  los_hwi
 * @brief Delete hardware interrupt.
 *
 * @par Description:
 * This API is used to delete hardware interrupt.
 *
 * @attention
 * <ul>
 * <li>The hardware interrupt module is usable only when the configuration item for hardware interrupt tailoring is enabled.</li>
 * <li>Hardware interrupt number value range: [OS_HWI_MIN,OS_HWI_MAX]. The value range applicable for a Cortex-M4 platform is [0,240].</li>
 * <li>OS_HWI_MAX_NUM specifies the maximum number of interrupts that can be created.</li>
 * <li>Before executing an interrupt on a platform, refer to the chip manual of the platform.</li>
 * </ul>
 *
 * @param  uwHwiNum   [IN] Type#HWI_HANDLE_T: hardware interrupt number. The value range applicable for a Cortex-M4 platform is [0,240].
 *
 * @retval #OS_ERRNO_HWI_NUM_INVALID              0x02000900: Invalid interrupt number.
 * @retval #LOS_OK                                  0: The interrupt is successfully delete.
 * @par Dependency:
 * <ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_HwiDelete(HWI_HANDLE_T uwHwiNum);

/**
 *@ingroup los_hwi
 *@brief Get interrupt number.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to get irq number .</li>
 *</ul>
 *@attention
 *<ul>
 *<li>This API can be called only when an irq come up .</li>
 *</ul>
 *
 *@param None.
 *
 *@retval UINT32 irq number.
 *@par Dependency:
 *<ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 *@see None
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_IntNumGet(VOID);

/**
 *@ingroup los_hwi
 *@brief diable interrupts.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to disable interrupts .</li>
 *</ul>
 *@attention
 *<ul>
 *<li>This API can be called only in osTaskExit() .</li>
 *</ul>
 *
 *@param None.
 *
 *@retval None.
 *@par Dependency:
 *<ul><li>los_hwi.h: the header file that contains the API declaration.</li></ul>
 *@see None
 *@since Huawei LiteOS V100R001C00
 */
extern VOID osDisableIRQ(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */



#endif /* _LOS_HWI_H */

