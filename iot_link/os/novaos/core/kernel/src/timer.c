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

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <common.h>
#include <class.h>
#include <irq.h>
#include <timer.h>
#include <task.h>
#include <tick.h>
#include <critical.h>
#include <errno.h>
#include <init.h>
#include <warn.h>

/* globals */

/*
 * classes are defined as an array with only 1 element, so we can use it as an
 * pointer
 */

class_t timer_class [1];

/**
 * timer_init - initialize a timer
 * @timer:    the timer to be initialized
 * @flags:    timer options
 * @interval: timer interval
 * @pfn:      timeout callback
 * @arg:      the argument for the timeout callback
 *
 * return: 0 on success, negtive value on error
 */

int timer_init (timer_id timer, uint16_t flags, unsigned long interval,
                void (* pfn) (uintptr_t), uintptr_t arg)
    {
    WARN_ON (timer == NULL,
             errno = ERRNO_TIMER_ILLEGAL_ID; return -1,
             "Invalid timer id!");

    return obj_init (timer_class, &timer->obj, flags, interval, pfn, arg);
    }

/**
 * timer_create - create a timer
 * @flags:    timer options
 * @interval: timer interval
 * @pfn:      timeout callback
 * @arg:      the argument for the timeout callback
 *
 * return: 0 on success, negtive value on error
 */

timer_id timer_create (uint16_t flags, unsigned long interval,
                       void (* pfn) (uintptr_t), uintptr_t arg)
    {

    /*
     * obj is just the first member of timer, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj_create (timer_class, flags, interval, pfn, arg),
                         timer_t, obj);
    }

static void __timer_tick_q_callback (struct tick_q_node * node, uintptr_t arg)
    {
    timer_id timer = container_of (node, timer_t, tq_node);

    (void) arg;

    /* __timer_tick_q_callback invoked in critical context */

    if (unlikely (obj_verify (timer_class, &timer->obj) != 0))
        {
        return;
        }

    /* fake interrupt context when invoking the callback */

    int_cnt++;

    timer->pfn (timer->arg);

    int_cnt--;

    if (timer->flags == TIMER_FLAG_REPEATED)
        {
        tick_q_add (&timer->tq_node, timer->interval, __timer_tick_q_callback, 0);

        timer->status = TIMER_STAT_ACTIVE;
        }
    else
        {
        timer->status = TIMER_STAT_INACTIVE;
        }
    }

static int __timer_start (uintptr_t arg1, uintptr_t arg2)
    {
    timer_id timer = (timer_id) arg1;

    (void) arg2;

    if (unlikely (timer->status == TIMER_STATUS_DELETING))
        {
        return -1;
        }

    if (unlikely (timer->status == TIMER_STAT_ACTIVE))
        {
        tick_q_del (&timer->tq_node);
        }

    tick_q_add (&timer->tq_node, timer->interval, __timer_tick_q_callback, 0);

    timer->status = TIMER_STAT_ACTIVE;

    return 0;
    }

/**
 * timer_start - start a timer
 * @timer: the timer id to be started
 *
 * return: 0 on success, negtive value on error
 */

int timer_start (timer_id timer)
    {
    int ret;

    WARN_ON (timer == NULL,
             errno = ERRNO_TIMER_ILLEGAL_ID; return -1,
             "Invalid timer id!");

    if (unlikely (obj_verify_protect (timer_class, &timer->obj) != 0))
        {
        return -1;
        }

    ret = do_critical (__timer_start, (uintptr_t) timer, 0);

    obj_unprotect (&timer->obj);

    return ret;
    }

static int __timer_stop (uintptr_t arg1, uintptr_t arg2)
    {
    timer_id timer = (timer_id) arg1;

    (void) arg2;

    if (timer->status != TIMER_STAT_ACTIVE)
        {
        return 0;
        }

    tick_q_del (&timer->tq_node);

    timer->status = TIMER_STAT_INACTIVE;

    return 0;
    }

/**
 * timer_start - stop a timer
 * @timer: the timer id to be stopped
 *
 * return: 0 on success, negtive value on error
 */

int timer_stop (timer_id timer)
    {
    int ret;

    WARN_ON (timer == NULL,
             errno = ERRNO_TIMER_ILLEGAL_ID; return -1,
             "Invalid timer id!");

    if (unlikely (obj_verify_protect (timer_class, &timer->obj) != 0))
        {
        return -1;
        }

    ret = do_critical (__timer_stop, (uintptr_t) timer, 0);

    obj_unprotect (&timer->obj);

    return ret;
    }

/* stop and mark the timer as deleting */

static int __timer_delete (uintptr_t arg1, uintptr_t arg2)
    {
    timer_id timer = (timer_id) arg1;

    (void) arg2;

    if (timer->status == TIMER_STAT_ACTIVE)
        {
        tick_q_del (&timer->tq_node);
        }

    timer->status = TIMER_STATUS_DELETING;

    return 0;
    }

/**
 * timer_delete - delete a timer
 * @timer: the timer id to be deleted
 *
 * return: 0 on success, negtive value on error
 */

int timer_delete (timer_id timer)
    {
    int ret;

    WARN_ON (timer == NULL,
             errno = ERRNO_TIMER_ILLEGAL_ID; return -1,
             "Invalid timer id!");

    if (unlikely (obj_verify_protect (timer_class, &timer->obj) != 0))
        {
        return -1;
        }

    /* this routine is restricted to be invoked in task context */

    ret = do_critical_non_irq (__timer_delete, (uintptr_t) timer, 0);

    obj_unprotect (&timer->obj);

    if (likely (ret == 0))
        {
        ret = obj_destroy (timer_class, &timer->obj);
        }

    return ret;
    }

static int __timer_init (obj_id obj, va_list valist)
    {
    timer_id      timer = container_of (obj, timer_t, obj);;
    unsigned int  flags;
    unsigned long interval;
    void       (* pfn) (uintptr_t);
    uintptr_t     arg;

    /* in fact flags is just an uint16_t */

    flags    = va_arg (valist, unsigned int);
    interval = va_arg (valist, unsigned long);
    pfn      = va_arg (valist, void (*) (uintptr_t));
    arg      = va_arg (valist, uintptr_t);

    WARN_ON (flags > TIMER_FLAG_REPEATED,
             errno = ERRNO_TIMER_ILLEGAL_FLAG;     return -1,
             "Invalid flag!");

    WARN_ON (interval == 0,
             errno = ERRNO_TIMER_ILLEGAL_INTERVAL; return -1,
             "Invalid interval!");

    WARN_ON (pfn == NULL,
             errno = ERRNO_TIMER_ILLEGAL_PFN;      return -1,
             "Invalid callback!");

    timer->status   = TIMER_STAT_INACTIVE;
    timer->flags    = (uint16_t) flags;
    timer->interval = interval;
    timer->pfn      = pfn;
    timer->arg      = arg;

    return 0;
    }

static int __timer_destroy (obj_id obj)
    {
    return 0;
    }

/**
 * timer_lib_init - timer library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int timer_lib_init (void)
    {
    if (class_init (timer_class, MID_TIMER, sizeof (timer_t),
                    __timer_init, __timer_destroy, NULL, NULL) != 0)
        {
        WARN ("Fail to init timer_class!");
        return -1;
        }

    return 0;
    }

MODULE_INIT (kernel, timer_lib_init);

