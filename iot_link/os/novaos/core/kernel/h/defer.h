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

#ifndef __DEFER_H__
#define __DEFER_H__

#include <stdint.h>

#include <list.h>
#include <atomic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define ERRNO_DEFERRED_ILLEGAL_JOB          ERRNO_JOIN (MID_DEFER, 1)
#define ERRNO_DEFERRED_ILLEGAL_PFN          ERRNO_JOIN (MID_DEFER, 2)

/**
 * DEFERRED_JOB_INIT - create a initialized deferred job body
 */

#define DEFERRED_JOB_INIT(pfn, arg)         \
    {                                       \
    NULL,                                   \
    (void  (*) (uintptr_t)) pfn,            \
    (uintptr_t) arg,                        \
    { 0 }                                   \
    }

/* typedefs */

typedef struct deferred_job
    {
    struct deferred_job * next;
    void               (* pfn) (uintptr_t);
    uintptr_t             arg;
    atomic_uint           busy;
    } deferred_job_t;

/* externs */

extern int deferred_job_sched (deferred_job_t *);
extern int deferred_job_init  (deferred_job_t *, void (*) (uintptr_t), uintptr_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DEFER_H__ */

