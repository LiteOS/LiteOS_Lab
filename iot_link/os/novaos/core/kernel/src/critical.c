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

#include <stdbool.h>
#include <stdint.h>

#include <common.h>
#include <compiler.h>
#include <irq.h>
#include <bug.h>
#include <init.h>
#include <task.h>
#include <critical.h>
#include <errno.h>
#include <warn.h>

#ifdef CONFIG_PROFILE
#include <profile.h>
#endif

#include <arch/interface.h>
#include <arch/sync.h>

/* globals */

struct critical_q critical_q = {0};

/* inlines */

static __always_inline bool __critical_q_full (void)
    {
    return (critical_q.tail_idx - critical_q.head_idx) == CRITICAL_JOB_Q_SLOTS;
    }

static __always_inline void __critical_enter (void)
    {
    critical_q.in_critical = true;
    mb ();
    }

static __always_inline void __critical_exit (void)
    {
    critical_q.in_critical = false;
    mb ();
    }

static inline int __critical_exec (int (* job) (uintptr_t, uintptr_t),
                                   uintptr_t arg1, uintptr_t arg2)
    {
    if (int_context ())
        {
        return job (arg1, arg2);
        }

    return critical_exec_on_csp (job, arg1, arg2);
    }

/**
 * __critical_q_add - add a job to the critical queue
 * @job:  the job routine
 * @arg1: the first argument
 * @arg2: the second argument
 *
 * return: 0 on success, -1 if queue full
 */

static __always_inline int __critical_q_add (int (* job) (uintptr_t, uintptr_t),
                                             uintptr_t arg1, uintptr_t arg2)
    {
    unsigned int  idx;
    unsigned long key;

    key = int_lock ();

    if (unlikely (__critical_q_full ()))
        {
        idx = (unsigned int) -1;
        }
    else
        {
        idx = critical_q.tail_idx++;
        mb ();
        }

    int_restore (key);

    if (unlikely (idx == (unsigned int) -1))
        {
        errno = ERRNO_CRITICAL_QUEUE_FULL;
        return -1;
        }

    idx &= CRITICAL_JOB_Q_MASK;

    /*
     * it is safe do this out of int_lock/int_restore protection:
     * 1) there are only two cases here:
     *    a) it is in task context now, one parent routine is just processing
     *       the critical queue
     *    b) it is in irq handler now, and this irq is just interruptted a task,
     *       or a lower priority irq and this task/irq handler is just procesing
     *       the critical queue
     * 2) the critical queue processing can only continue when current routine
     *    or irq handler return
     * 3) at that time, the followint assignment is done, of cause
     */

    critical_q.jobs [idx].pfn  = job;
    critical_q.jobs [idx].arg1 = arg1;
    critical_q.jobs [idx].arg2 = arg2;

    return 0;
    }

/**
 * critical_exec - exec a job and then walk through the critical queue
 *
 * return: return value of the job
 */

__weak int critical_exec (int (* job) (uintptr_t, uintptr_t),
                          uintptr_t arg1, uintptr_t arg2)
    {
    int                   ret;
    unsigned long         key;
    unsigned int          itr;
    struct critical_job * critical_job;

    if (unlikely (critical_q.in_critical))
        {
        return __critical_q_add (job, arg1, arg2);
        }

    __critical_enter ();

    ret = __critical_exec (job, arg1, arg2);

    itr = critical_q.head_idx;

#ifdef CONFIG_PROFILE

    /* any delayed critical job must came from interrupt, so they belongs to sys */

    profile_sys_enter ();
#endif

    while (1)
        {
        key = int_lock ();

        if (likely (itr == critical_q.tail_idx))
            {
            __critical_exit ();
            int_restore (key);
            break;
            }
        else
            {
            int_restore (key);
            }

        critical_job = &critical_q.jobs [itr & CRITICAL_JOB_Q_MASK];

        (void) __critical_exec (critical_job->pfn,
                                critical_job->arg1, critical_job->arg2);

        /* prevent head_idx is increase before the job invoking */

        mb ();

        critical_q.head_idx = ++itr;

        mb ();
        }

#ifdef CONFIG_PROFILE
    profile_sys_exit ();
#endif

#ifdef CONFIG_CRITICAL_STACK_CHECK

    /* check if critical stack overflow */

    BUG_ON (critical_q.magic != CRITICAL_MAGIC, "critical stack overflow!");
#endif

    /*
     * schedule will just return the 'ret' passed to it, but this value
     * may be changed though task_retval_set
     */

    return schedule (ret);
    }

/**
 * do_critical - do critical work, can be invoked in all context
 * @job:  the job routine
 * @arg1: the first argument
 * @arg2: the second argument
 *
 * return: status
 */

int do_critical (int (* job) (uintptr_t, uintptr_t),
                 uintptr_t arg1, uintptr_t arg2)
    {
    return critical_exec (job, arg1, arg2);
    }

static __always_inline bool __no_sleepable (void)
    {
    return current == NULL || task_lock_cnt || int_context () || int_locked () ||
           (current->option & TASK_OPTION_NO_BLOCK) || critical_q.in_critical;
    }

/**
 * do_critical_might_sleep - do critical that may cause the caller sleep
 * @job:  the job routine
 * @arg1: the first argument
 * @arg2: the second argument
 *
 * return: status
 */

int do_critical_might_sleep (int (* job) (uintptr_t, uintptr_t),
                             uintptr_t arg1, uintptr_t arg2)
    {
    WARN_ON (__no_sleepable (),
             errno = ERRNO_CRITICAL_ILLEGAL_OPERATION; return -1,
             "do_critical_might_sleep () invoked from atomic context!");

    return critical_exec (job, arg1, arg2);
    }

/**
 * do_critical_non_irq - do critical job in irq restricted context
 * @job:  the job routine
 * @arg1: the first argument
 * @arg2: the second argument
 *
 * return: status
 */

int do_critical_non_irq (int (* job) (uintptr_t, uintptr_t),
                         uintptr_t arg1, uintptr_t arg2)
    {
    WARN_ON (int_context (),
             errno = ERRNO_CRITICAL_ILLEGAL_OPERATION; return -1,
             "do_critical_non_irq () invoked from ISR context!");

    return critical_exec (job, arg1, arg2);
    }

/**
 * sleepable - check if current context sleepable
 *
 * return: true if sleepable, false if not
 */

bool sleepable (void)
    {
    return !__no_sleepable ();
    }

#ifdef CONFIG_CRITICAL_STACK_CHECK

/**
 * critical_lib_init - critical library initialization routine
 *
 * return: NA
 */

static int critical_lib_init (void)
    {
    critical_q.magic = CRITICAL_MAGIC;

    return 0;
    }

MODULE_INIT (kernel, critical_lib_init);

#endif

