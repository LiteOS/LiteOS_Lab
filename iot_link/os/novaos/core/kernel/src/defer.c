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

#include <stdlib.h>
#include <limits.h>

#include <common.h>
#include <critical.h>
#include <defer.h>
#include <task.h>
#include <init.h>
#include <warn.h>
#include <bug.h>

/* locals */

static task_t                    __defer [1];
static uint64_t                  __defer_stack [CONFIG_DEFERRED_STACK_SIZE /
                                                sizeof (uint64_t)];
static deferred_job_t *          __deferred_head = NULL;
static deferred_job_t *          __deferred_tail = NULL;
static deferred_job_t * volatile __deferred_next = (deferred_job_t *) -1;

/* globals */

/* typedefs */

/**
 * deferred_job_init - initialize a deferred job
 * @job: the job to be initialized
 * @pfn: the callback to be run deffered
 * @arg: the argument to the callback
 *
 * return: 0 on success, negtive value on error
 */

int deferred_job_init (deferred_job_t * job, void (* pfn) (uintptr_t),
                       uintptr_t arg)
    {
    WARN_ON (job == NULL,
             errno = ERRNO_DEFERRED_ILLEGAL_JOB; return -1,
             "Invalid job!");

    WARN_ON (pfn == NULL,
             errno = ERRNO_DEFERRED_ILLEGAL_PFN; return -1,
             "Invalid pfn!");

    job->pfn = pfn;
    job->arg = arg;

    atomic_uint_set (&job->busy, 0);

    return 0;
    }

static int __deferred_put (uintptr_t arg1, uintptr_t arg2)
    {
    deferred_job_t * job = (deferred_job_t *) arg1;

    (void) arg2;

    if ((__defer->status != TASK_STATUS_READY) &&
        (__deferred_next == NULL))
        {
        __deferred_next = (void *) job;
        task_ready_q_add (__defer);

        return 0;
        }

    if (__deferred_head == NULL)
        {
        __deferred_head = job;
        }
    else
        {
        __deferred_tail->next = job;
        }

    __deferred_tail = job;

    return 0;
    }

/**
 * deferred_job_sched - schedule a deferred job to run
 * @job:  the job to run
 *
 * return: 0 on success, negtive value on error
 */

int deferred_job_sched (deferred_job_t * job)
    {
    WARN_ON (job      == NULL,
             errno = ERRNO_DEFERRED_ILLEGAL_JOB; return -1,
             "Invalid job!");

    WARN_ON (job->pfn == NULL,
             errno = ERRNO_DEFERRED_ILLEGAL_PFN; return -1,
             "Invalid pfn!");

    /* job already in queue */

    if (unlikely (!atomic_uint_set_eq (&job->busy, 0, 1)))
        {
        return -1;
        }

    job->next = NULL;

    return do_critical (__deferred_put, (uintptr_t) job, 0);
    }

static int __deferred_get (uintptr_t arg1, uintptr_t arg2)
    {
    (void) arg1;
    (void) arg2;

    if (__deferred_head == NULL)
        {
        task_ready_q_del (current);
        current->status = TASK_STATUS_PEND;
        }
    else
        {
        __deferred_next = __deferred_head;
        __deferred_head = __deferred_head->next;

        /* if __deferred_head is NULL, it is ok left __deferred_tail as it is */
        }

    return 0;
    }

static __noreturn int deferred_task (uintptr_t dummy)
    {
    void          (* pfn) (uintptr_t);
    uintptr_t        arg;
    deferred_job_t * job;

    while (1)
        {
        __deferred_next = NULL;

        do_critical (__deferred_get, 0, 0);

        job = __deferred_next;

        pfn = job->pfn;
        arg = job->arg;

        atomic_uint_set (&job->busy, 0);

        pfn (arg);
        }
    }

/**
 * deferred_lib_init - defer initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int deferred_lib_init (void)
    {
    BUG_ON (task_init (__defer, (char *) __defer_stack, CONFIG_DEFERRED_NAME, 0,
                       TASK_OPTION_SYSTEM, CONFIG_DEFERRED_STACK_SIZE,
                       deferred_task, 0) != 0,
            "Fail to initialize deferred task!");

    BUG_ON (task_resume (__defer) != 0,
            "Fail to resume deffered task!");

    return 0;
    }

MODULE_INIT (postkernel, deferred_lib_init);

