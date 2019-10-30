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
#include <stdint.h>

#include <common.h>
#include <class.h>
#include <list.h>
#include <event.h>
#include <task.h>
#include <critical.h>
#include <errno.h>
#include <init.h>
#include <warn.h>
#include <bug.h>

#ifdef CONFIG_SYSCALL
#include <syscall.h>
#endif

/* typedefs */

struct event_data
    {
    uint32_t event_wanted;
    uint32_t event_recved;
    uint32_t event_option;
    };

/* globals */

/*
 * classes are defined as an array with only 1 element, so we can use it as an
 * pointer
 */

class_t event_class [1];

/**
 * event_init - initialize an event
 * @event: the event id to be initialized
 *
 * return: 0 on success, negtive value on error
 */

int event_init (event_id event)
    {
    WARN_ON (event == NULL,
             errno = ERRNO_EVENT_ILLEGAL_ID; return -1,
             "Invalid event object!");

    return obj_init (event_class, &event->obj);
    }

/**
 * event_create - create an event
 *
 * return: the allocated event or NULL on error
 */

event_id event_create (void)
    {

    /*
     * obj is just the first member of event, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj_create (event_class), event_t, obj);
    }

/**
 * event_open - open a event with a given name
 * @name:  the name of the event
 * @oflag: flag, 0 or O_CREAT or O_CREAT | O_EXCL
 * @mode:  if oflag has O_CREAT option
 * @value: if oflag has O_CREAT option
 *
 * return: the event id or NULL on error
 */

event_id event_open (const char * name, int oflag, ...)
    {
    obj_id  obj;
    va_list valist;

    va_start (valist, oflag);

    obj = obj_open (event_class, name, oflag, valist);

    va_end (valist);

    /*
     * obj is just the first member of event, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj, event_t, obj);
    }

/**
 * event_destroy - destroy a event
 * @event:   the event to be destroied
 *
 * return: 0 on success, negtive value on error
 */

int event_destroy (event_id event)
    {
    WARN_ON (event == NULL,
             errno = ERRNO_EVENT_ILLEGAL_ID; return -1,
             "Invalid event object!");

    return obj_destroy (event_class, &event->obj);
    }

static void __tick_q_callback_event (task_id task)
    {
    task->wanted.id = NULL;

    /* set the errno of the task */

    task_errno_set (task, ERRNO_EVENT_TIMEOUT);
    }

/**
 * __event_feed - feed a task waiting for events from an event
 * @task:    the task to feed, the task may be current (invoked from event_recv)
 *           or a task in the pend_q (invoked form event_send)
 * @event:   the event id, now holding event sets, and is the event that the task
 *           just want to read from
 * @timeout: max wait ticks, always 0 if invoked form event_send, trying to wake
 *           up a task
 *
 * return: 0 on success, negtive value on error
 */

static int __event_feed (task_id task, event_id event, unsigned int timeout)
    {
    struct event_data * data   = (struct event_data *) task->wanted.pdata;
    uint32_t            wanted = data->event_wanted;

    switch (data->event_option & (EVENT_WAIT_ALL | EVENT_WAIT_ANY))
        {
        case EVENT_WAIT_ALL:
            if ((event->event_set & wanted) == wanted)
                goto got;
            break;
        case EVENT_WAIT_ANY:
            if ((event->event_set & wanted) != 0)
                goto got;
            break;
        default:
            WARN ("Event option not supported!");
            errno = ERRNO_EVENT_ILLEGAL_OPERATION;
            return -1;
        }

    if (!timeout)
        {

        /* if feeding other task, means current is just invoking event_send */

        if (task == current)
            {
            errno = ERRNO_EVENT_UNAVAILABLE;
            }

        return -1;
        }

    task_pwait_q_add (&event->pend_q, timeout, __tick_q_callback_event);

    return 0;

got:

    data->event_recved = event->event_set & wanted;

    if (data->event_option & EVENT_WAIT_CLR)
        {
        event->event_set &= ~wanted;
        }

    return 0;
    }

static int __event_recv (uintptr_t arg1, uintptr_t arg2)
    {
    return __event_feed (current, (event_id) arg1, (unsigned int) arg2);
    }

/**
 * event_timedrecv - timed receive event(s) from an event object
 * @event:   the event id
 * @wanted:  wanted event set
 * @option:  the receive option, EVENT_WAIT_ALL or EVENT_WAIT_ANY
 * @timeout: max ticks wait events
 * @recved:  event set(s) received
 *
 * return: 0 on success, negtive value on error
 */

int event_timedrecv (event_id event, uint32_t wanted, uint32_t option,
                     unsigned int timeout, uint32_t * recved)
    {
    struct event_data data;
    int               ret;

    WARN_ON (event == NULL,
             errno = ERRNO_EVENT_ILLEGAL_ID; return -1,
             "Invalid event object!");

    /* check if option valid */

    switch (option & (EVENT_WAIT_ALL | EVENT_WAIT_ANY))
        {
        case EVENT_WAIT_ALL:
        case EVENT_WAIT_ANY:
            break;
        default:
            WARN ("Invalid event option!");
            errno = ERRNO_EVENT_ILLEGAL_OPERATION;
            return -1;
        }

    if (unlikely (obj_verify_protect (event_class, &event->obj) != 0))
        {
        return -1;
        }

    data.event_wanted = wanted;
    data.event_option = option;

    current->wanted.id    = &event->obj;
    current->wanted.pdata = (void *) &data;

    ret = do_critical_might_sleep (__event_recv, (uintptr_t) event,
                                   (uintptr_t) timeout);

    obj_unprotect (&event->obj);

    if ((ret == 0) && (recved != NULL))
        {
        *recved = data.event_recved;
        }

    return ret;
    }

/**
 * event_timedrecv - receive event(s) from an event object
 * @event:   the event id
 * @wanted:  wanted event set
 * @option:  the receive option, EVENT_WAIT_ALL or EVENT_WAIT_ANY
 * @recved:  event set(s) received
 *
 * return: 0 on success, negtive value on error
 */

int event_recv (event_id event, uint32_t wanted, uint32_t option, uint32_t * recved)
    {
    return event_timedrecv (event, wanted, option, WAIT_FOREVER, recved);
    }

/**
 * event_tryrecv - try receive event(s) from an event object
 * @event:   the event id
 * @wanted:  wanted event set
 * @option:  the receive option, EVENT_WAIT_ALL or EVENT_WAIT_ANY
 * @recved:  event set(s) received
 *
 * return: 0 on success, negtive value on error
 */

int event_tryrecv (event_id event, uint32_t wanted, uint32_t option, uint32_t * recved)
    {
    return event_timedrecv (event, wanted, option, NO_WAIT, recved);
    }

static int __event_send (uintptr_t arg1, uintptr_t arg2)
    {
    event_id  event  = (event_id) arg1;
    uint32_t  events = (uint32_t) arg2;
    dlist_t * itr;

    event->event_set |= events;

    dlist_foreach (itr, &event->pend_q)
        {
        task_id task = container_of (itr, task_t, pq_node);

        if (__event_feed (task, event, NO_WAIT))
            {
            continue;
            }

        task->wanted.id = NULL;

        task_ready_q_add (task);

        if (event->event_set == 0)
            {
            break;
            }
        }

    return 0;
    }

/**
 * event_send - send event(s) to an event object
 * @event:  the event id
 * @events: event set
 *
 * return: 0 on success, negtive value on error
 */

int event_send (event_id event, uint32_t events)
    {
    int ret;

    WARN_ON (event == NULL,
             errno = ERRNO_EVENT_ILLEGAL_ID; return -1,
             "Invalid event object!");

    WARN_ON (events == 0,
             errno = ERRNO_EVENT_ILLEGAL_OPERATION; return -1,
             "Invalid events!");

    if (unlikely (obj_verify_protect (event_class, &event->obj) != 0))
        {
        return -1;
        }

    ret = do_critical (__event_send, (uintptr_t) event, (uintptr_t) events);

    obj_unprotect (&event->obj);

    return ret;
    }

static int __event_init (obj_id obj, va_list valist)
    {
    event_id event = container_of (obj, event_t, obj);

    event->event_set = 0;
    dlist_init (&event->pend_q);

    return 0;
    }

static int __event_destroy_critical (uintptr_t arg1, uintptr_t arg2)
    {
    event_id  event = (event_id) arg1;
    dlist_t * itr, * n;

    (void) arg2;

    /*
     * we must come here from obj_destroy and object already destroyed
     * we do not need to and we can not verify and protect the object
     */

    /* task_resume will update the return value to -1 */

    dlist_foreach_safe (itr, n, &event->pend_q)
        {
        task_id task = container_of (itr, task_t, pq_node);

        task_resume (task);

        /* set the errno of the task */

        task_errno_set (task, ERRNO_EVENT_DELETED);
        }

    return 0;
    }

static int __event_destroy (obj_id obj)
    {

    /* this routine is always invoked under task context */

    return do_critical (__event_destroy_critical,
                        (uintptr_t) container_of (obj, event_t, obj), 0);
    }

/**
 * event_lib_init - event library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int event_lib_init (void)
    {
    BUG_ON (class_init (event_class, MID_EVENT, sizeof (event_t),
                        __event_init, __event_destroy, NULL, NULL),
            "fail to initialize event_class!");

    return 0;
    }

MODULE_INIT (kernel, event_lib_init);

#ifdef CONFIG_SYSCALL
static const uintptr_t syscall_entries_event [] =
    {
    (uintptr_t) event_create,
    (uintptr_t) event_open,
    (uintptr_t) event_destroy,
    (uintptr_t) event_timedrecv,
    (uintptr_t) event_recv,
    (uintptr_t) event_tryrecv,
    (uintptr_t) event_send,
    };

const struct syscall_table syscall_table_event =
    {
    ARRAY_SIZE (syscall_entries_event),
    syscall_entries_event
    };
#endif

