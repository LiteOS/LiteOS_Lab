/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#include <config.h>
#include <compiler.h>

/* externs */

extern void reset_handler   (void);
extern void exc_handler     (void);
extern void svc_handler     (void);
extern void pendsv_handler  (void);
extern void systick_handler (void);
extern void irq_handler     (void);

__section__ (vectors) __unused void (* const _vectors []) (void) =
    {
    NULL,
    reset_handler,          /* reset        */
    exc_handler,            /* NMI          */
    exc_handler,            /* HardFault    */
#ifdef  CONFIG_AARCH_M_BASELINE
    NULL,
    NULL,
    NULL,
#else
    exc_handler,            /* MemManage    */
    exc_handler,            /* BusFault     */
    exc_handler,            /* UsageFault   */
#endif
    NULL,
    NULL,
    NULL,
    NULL,
#ifdef CONFIG_SVC
    svc_handler,            /* SVCall       */
#else
    exc_handler,            /* SVCall       */
#endif
#ifdef  CONFIG_AARCH_M_BASELINE
    NULL,
#else
    exc_handler,            /* DebugMonitor */
#endif
    NULL,
    pendsv_handler,         /* PendSV       */
    systick_handler,        /* SysTick, NA  */

#if CONFIG_NR_IRQS != 0
    [16 ... 16 + CONFIG_NR_IRQS - 1] = irq_handler
#endif
    };
