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

#include <string.h>
#include <stdlib.h>

#include <sem.h>
#include <task.h>
#include <timer.h>
#include <hal_int.h>
#include <atomic.h>
#include <profile.h>

/* locals */

static struct profile_data   __system_profile;

/* task now beening measured */

static struct profile_data * __curdata;

static unsigned long         __tsc_period = 0;

static volatile bool         __counting = false;
static volatile bool         __done = false;

static sem_t                 __sync_sem;

static int                   __tls_slot = -1;

static atomic_uint           __sysenter_cnt;

static timer_t               __timer_starter;
static timer_t               __timer_stopper;

static unsigned long __calc_tsc (unsigned long old, unsigned long new)
    {
    if (unlikely (new < old))
        {
        return new + (__tsc_period - old);
        }
    else
        {
        return new - old;
        }
    }

static void __tsc_start (struct profile_data * data)
    {
    if (unlikely (data == __curdata))
        {
        return;
        }

    if ((__curdata != NULL) && (__curdata->tsc_started))
        {
        __curdata->tsc_all += __calc_tsc (__curdata->tsc_start, tick_tsc ());
        }

    data->tsc_start   = tick_tsc ();
    data->tsc_started = true;

    __curdata = data;
    }

static void __timer_func (uintptr_t arg)
    {
    __counting = (bool) arg;

    /* if (bool) arg is false, means counting done, wakeup top task */

    if (!((bool) arg))
        {
        __done = true;
        sem_post (&__sync_sem);
        }
    }

static int __task_reset (task_id task, uintptr_t arg)
    {
    struct profile_data * data;

    (void) arg;

    /* should never fail */

    (void) task_tls_get (task, __tls_slot, (uintptr_t *) &data);

    data->tsc_started = false;
    data->tsc_all     = 0;

    return 0;
    }

static void __system_reset (void)
    {
    __system_profile.tsc_started = false;
    __system_profile.tsc_all     = 0;
    }

static inline void __task_enter (task_id task)
    {
    struct profile_data * data;

    if (unlikely (task_tls_get (task, __tls_slot, (uintptr_t *) &data) != 0))
        {
        return;
        }

    __tsc_start (data);
    }

void profile_task_enter (task_id task)
    {
    if (likely (!__counting))
        {
        return;
        }

    __task_enter (task);
    }

void profile_sys_enter (void)
    {
    if (likely (!__counting))
        {
        return;
        }

    atomic_uint_inc (&__sysenter_cnt);

    __tsc_start (&__system_profile);
    }

void profile_sys_exit (void)
    {
    if (likely (!__counting))
        {
        return;
        }

    atomic_uint_dec_nz (&__sysenter_cnt);

    if (likely (atomic_uint_get (&__sysenter_cnt) == 0))
        {
        __task_enter (current);
        }
    }

static int __create_hook (task_id task)
    {
    struct profile_data * data = (struct profile_data *) malloc (sizeof (struct profile_data));

    if (unlikely (data == NULL))
        {
        return -1;
        }

    data->tsc_started = false;
    data->tsc_all     = 0;
    data->task        = task;

    (void) task_tls_set (task, __tls_slot, (uintptr_t) data);

    return 0;
    }

static void __delete_hook (task_id task)
    {
    struct profile_data * data;

    if (unlikely (task_tls_get (task, __tls_slot, (uintptr_t *) &data) != 0))
        {
        return;
        }

    task_tls_set (task, __tls_slot, 0);

    free (data);
    }

static void __switch_hook (task_id old, task_id new)
    {
    (void) old;

    profile_task_enter (new);
    }

static int __profile_init (void)
    {
    static bool inited = false;

    if (inited)
        {
        return 0;
        }

    (void) timer_init (&__timer_stopper, TIMER_FLAG_ONE_SHOT, 1 + CONFIG_SYS_TICK_HZ,
                       __timer_func, (uintptr_t) false);
    (void) timer_init (&__timer_starter, TIMER_FLAG_ONE_SHOT, 1,
                       __timer_func, (uintptr_t) true);

    (void) sem_init   (&__sync_sem, 0);

    __system_profile.task = NULL;

    __tsc_period = tick_tsc_period ();

    if (unlikely (__tsc_period == 0))
        {
        return -1;
        }

    __tls_slot = task_tls_slot_alloc ();

    if (unlikely (__tls_slot == -1))
        {
        return -1;
        }

    if (unlikely (task_delete_hook_add (__delete_hook) != 0))
        {
        return -1;
        }

    if (unlikely (task_create_hook_add (__create_hook) != 0))
        {
        return -1;
        }

    if (unlikely (task_switch_hook_add (__switch_hook) != 0))
        {
        return -1;
        }

    inited = true;

    return 0;
    }

int profile_start (void)
    {

    if (unlikely (__profile_init () != 0))
        {
        return -1;
        }

    if (unlikely (task_foreach (__task_reset, 0) != 0))
        {
        return -1;
        }

    __system_reset ();

    __curdata = NULL;

    __done = false;

    atomic_uint_set (&__sysenter_cnt, 0);

    /* should never fail */

    /* make sure the following two timer_start executived in the same tick */

    task_delay (1);

    timer_start (&__timer_stopper);
    timer_start (&__timer_starter);

    sem_wait (&__sync_sem);

    return 0;
    }

static int __task_foreach (task_id task, uintptr_t arg)
    {
    void               (* callback) (struct profile_data *, uintptr_t);
    uintptr_t           * args = (uintptr_t *) arg;
    struct profile_data * data;

    if (likely (task_tls_get (task, __tls_slot, (uintptr_t *) &data) == 0))
        {
        callback = (void (*) (struct profile_data *, uintptr_t)) args [0];

        callback (data, args [1]);
        }

    return 0;
    }

int profile_foreach (void (* callback) (struct profile_data *, uintptr_t), uintptr_t arg)
    {
    uintptr_t pfn_arg [2] = {(uintptr_t) callback, arg};

    if (unlikely (__counting || !__done || callback == NULL))
        {
        return -1;
        }

    callback (&__system_profile, arg);

    task_foreach (__task_foreach, (uintptr_t) &pfn_arg [0]);

    return 0;
    }
