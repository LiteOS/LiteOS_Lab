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

#ifndef __CRITICAL_H__
#define __CRITICAL_H__

#include <stdint.h>
#include <stdbool.h>

#include <module.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define CRITICAL_JOB_Q_SLOTS                (CONFIG_NR_CRITICAL_JOB_Q_SLOTS)
#define CRITICAL_JOB_Q_MASK                 (CRITICAL_JOB_Q_SLOTS - 1)

#define ERRNO_CRITICAL_QUEUE_FULL           ERRNO_JOIN (MID_CRITICAL, 1)
#define ERRNO_CRITICAL_ILLEGAL_OPERATION    ERRNO_JOIN (MID_CRITICAL, 2)

#ifdef CONFIG_CRITICAL_STACK_CHECK
#define CRITICAL_MAGIC                      0x466348614974516aull
#endif

/* typedefs */

struct critical_job
    {
    int                (* pfn) (uintptr_t, uintptr_t);
    uintptr_t             arg1;
    uintptr_t             arg2;
    uintptr_t             pad;
    };

#if (CONFIG_CRITICAL_STACK_SIZE & 7) != 0
#error "critical stack must be at least 8-byte aligned"
#endif

struct critical_q
    {
#ifdef CONFIG_CRITICAL_STACK_CHECK
    uint64_t              magic;
#endif
    uint64_t              stack [CONFIG_CRITICAL_STACK_SIZE >> 3];
    volatile bool         in_critical;
    volatile unsigned int head_idx;
    volatile unsigned int tail_idx;
    volatile unsigned int pad;
    struct critical_job   jobs [CRITICAL_JOB_Q_SLOTS];
    };

/* externs */

extern struct critical_q  critical_q;

extern int  do_critical_might_sleep (int (*) (uintptr_t, uintptr_t), uintptr_t, uintptr_t);
extern int  do_critical             (int (*) (uintptr_t, uintptr_t), uintptr_t, uintptr_t);
extern int  do_critical_non_irq     (int (*) (uintptr_t, uintptr_t), uintptr_t, uintptr_t);
extern bool sleepable               (void);

/* inlines */

static __always_inline bool in_critical (void)
    {
    return critical_q.in_critical;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CRITICAL_H__ */

