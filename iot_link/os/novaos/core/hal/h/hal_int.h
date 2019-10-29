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

#ifndef __HAL_INT_H__
#define __HAL_INT_H__

#include <stdint.h>

#include <module.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define ERRNO_HAL_INTC_ILLEGAL_OPERATION    ERRNO_JOIN (MID_HAL, 1)
#define ERRNO_HAL_INTC_ILLEGAL_IRQN         ERRNO_JOIN (MID_HAL, 2)

/* typedefs */

typedef void (* hal_int_handler_t) (uintptr_t);

typedef struct hal_int_methods
    {
    int  (* enable)            (unsigned int irq);
    int  (* disable)           (unsigned int irq);
    int  (* setprio)           (unsigned int irq, unsigned int prio);
    void (* handler)           (void);
    } hal_int_methods_t;

/* externs */

extern void hal_int_dispatch   (unsigned int irq);
extern int  hal_int_connect    (unsigned int irq, hal_int_handler_t handler,
                                uintptr_t arg);
extern int  hal_int_disconnect (unsigned int irq);
extern int  hal_int_setprio    (unsigned int irq, unsigned int prio);
extern int  hal_int_enable     (unsigned int irq);
extern int  hal_int_disable    (unsigned int irq);
extern int  hal_int_register   (const hal_int_methods_t * methods);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HAL_INTC___ */

