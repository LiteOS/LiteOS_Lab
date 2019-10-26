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
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#include <common.h>
#include <class.h>
#include <mutex.h>
#include <critical.h>
#include <irq.h>
#include <errno.h>
#include <init.h>
#include <warn.h>
#include <bug.h>

#ifdef CONFIG_SYSCALL
#include <syscall.h>
#endif

/* globals */

/*
 * classes are defined as an array with only 1 element, so we can use it as an
 * pointer
 */

class_t mutex_class [1];

/**
 * mutex_init - initialize a mutex
 * @mutex: the mutex to be initialized
 *
 * return: 0 on success, negtive value on error
 */

int mutex_init (mutex_id mutex)
    {
    WARN_ON (mutex == NULL,
             errno = ERRNO_MUTEX_ILLEGAL_ID; return -1,
             "Invalid mutex id");

    return obj_init (mutex_class, &mutex->obj);
    }

/**
 * mutex_create - create a mutex
 *
 * return: the allocated mutex or NULL on error
 */

mutex_id mutex_create (void)
    {

    /*
     * obj is just the first member of mutex, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj_create (mutex_class), mutex_t, obj);
    }

/**
 * mutex_open - open a mutex with a given name
 * @name:  the name of the mutex
 * @oflag: flag, 0 or O_CREAT or O_CREAT | O_EXCL
 * @mode:  if oflag has O_CREAT option
 * @value: if oflag has O_CREAT option
 *
 * return: the mutex id or NULL on error
 */

mutex_id mutex_open (const char * name, int oflag, ...)
    {
    obj_id  obj;
    va_list valist;

    va_start (valist, oflag);

    obj = obj_open (mutex_class, name, oflag, valist);

    va_end (valist);

    /*
     * obj is just the first member of mutex, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj, mutex_t, obj);
    }

static inline bool __recalc_task_prio (task_id task)
    {
    mutex_id  mutex;
    dlist_t * itr;
    uint8_t   prio = task->o_prio;

    if (task->c_prio == prio)
        {
        return false;
        }

    dlist_foreach (itr, &task->mutex_owned)
        {
        mutex = container_of (itr, mutex_t, node);

        if (mutex->prio < prio)
            {
            prio = mutex->prio;
            }
        }

    if (prio == task->c_prio)
        {
        return false;
        }

    if (task->status == TASK_STATUS_READY)
        {
        task_ready_q_del (task);
        task->c_prio = prio;
        task_ready_q_ins (task);
        }
    else
        {
        task->c_prio = prio;
        }

    return true;
    }

/**
 * mutex_destroy - destroy a mutex
 * @mutex:   the mutex to be destroied
 *
 * return: 0 on success, negtive value on error
 */

int mutex_destroy (mutex_id mutex)
    {
    int ret;

    if (unlikely (mutex_trylock (mutex) != 0))
        {
        return -1;
        }

    ret = obj_destroy (mutex_class, &mutex->obj);

    if (errno == ERRNO_OBJ_STILL_USING)
        {
        mutex_unlock (mutex);
        }

    return ret;
    }

static inline void __try_raise_mutex_prio (mutex_id mutex, uint8_t prio)
    {
    while (mutex->prio > prio)
        {
        task_id owner = mutex->owner;

        mutex->prio = prio;

        /* owner's prio may be higher */

        if (prio >= owner->c_prio)
            {
            return;
            }

        /* owner in ready_q and prio changed */

        if (owner->status == TASK_STATUS_READY)
            {
            task_ready_q_del (owner);
            owner->c_prio = prio;
            task_ready_q_add (owner);
            /* must not pend on mutex, just return */
            return;
            }

        owner->c_prio = prio;

        if (owner->wanted.id == NULL || owner->wanted.id->class->mid != MID_MUTEX)
            {
            return;
            }

        mutex = container_of (owner->wanted.id, mutex_t, obj);

        /*
         * the owner is pended on a mutex, and its priority is changed, so
         * correct its position in the pend queue
         */

        task_pwait_q_adj (&mutex->pend_q, owner);
        }
    }

static void __recalc_mutex_prio (mutex_id mutex)
    {
    if (dlist_empty (&mutex->pend_q))
        {
        mutex->prio = TASK_PRIO_MAX;
        }
    else
        {
        mutex->prio = container_of (mutex->pend_q.next, task_t, pq_node)->c_prio;
        }
    }

static void __mutex_set_owner (mutex_id mutex, task_id owner)
    {
    mutex->recurse = 1;
    mutex->owner   = owner;

    __recalc_mutex_prio (mutex);

    dlist_add (&owner->mutex_owned, &mutex->node);
    }

static inline void __try_lower_mutex_prio (mutex_id mutex)
    {
    while (1)
        {
        uint8_t prio = mutex->prio;
        task_id owner;

        __recalc_mutex_prio (mutex);

        if (prio == mutex->prio)
            {
            return;
            }

        if ((owner = mutex->owner) == NULL)
            {
            return;
            }

        if (!__recalc_task_prio (owner))
            {
            return;
            }

        if (owner->wanted.id == NULL || owner->wanted.id->class->mid != MID_MUTEX)
            {
            return;
            }

        mutex = container_of (owner->wanted.id, mutex_t, obj);

        /*
         * the owner is pended on a mutex, and its priority is changed, so
         * correct its position in the pend queue
         */

        task_pwait_q_adj (&mutex->pend_q, owner);
        }
    }

static void __tick_q_callback_mutex (task_id task)
    {
    mutex_id mutex = container_of (task->wanted.id, mutex_t, obj);
    uint8_t  prio  = mutex->prio;

    task->wanted.id = NULL;

    /* set the errno of the task */

    task_errno_set (task, ERRNO_MUTEX_TIMEOUT);

    /* current task is not the highest priority task pending in this mutex */

    if (prio != task->c_prio)
        {
        return;
        }

    /* task->c_prio needless change */

    __try_lower_mutex_prio (mutex);
    }

static int __mutex_lock (uintptr_t arg1, uintptr_t arg2)
    {
    mutex_id     mutex   = (mutex_id) arg1;
    unsigned int timeout = (unsigned int) arg2;

    if (mutex->recurse == 0)
        {
        __mutex_set_owner (mutex, current);

        return 0;
        }

    if (mutex->owner == current)
        {
        mutex->recurse++;

        return 0;
        }

    if (timeout == 0)
        {
        errno = ERRNO_MUTEX_UNAVAILABLE;
        return -1;
        }

    __try_raise_mutex_prio (mutex, current->c_prio);

    current->wanted.id = &mutex->obj;

    task_pwait_q_add (&mutex->pend_q, timeout, __tick_q_callback_mutex);

    return 0;
    }

/**
 * mutex_timedlock - lock a mutex with timeout
 * @mutex:   the mutex to be locked
 * @timeout: the max number of waiting ticks
 *
 * return: 0 on success, negtive value on error
 */

int mutex_timedlock (mutex_id mutex, unsigned int timeout)
    {
    int ret;

    WARN_ON (mutex == NULL,
             errno = ERRNO_MUTEX_ILLEGAL_ID; return -1,
             "Invalid mutex id");

    if (unlikely (current == NULL))
        {
        return 0;       /* pre-kernel, no racing */
        }

    if (unlikely (obj_verify_protect (mutex_class, &mutex->obj) != 0))
        {
        return -1;
        }

    ret = do_critical_might_sleep (__mutex_lock, (uintptr_t) mutex,
                                   (uintptr_t) timeout);

    obj_unprotect (&mutex->obj);

    return ret;
    }

/**
 * mutex_lock - lock a mutex
 * @mutex:   the mutex to be locked
 *
 * return: 0 on success, negtive value on error
 */

int mutex_lock (mutex_id mutex)
    {
    return mutex_timedlock (mutex, WAIT_FOREVER);
    }

/**
 * mutex_trylock - try to lock a mutex
 * @mutex:   the mutex to be locked
 *
 * return: 0 on success, negtive value on error
 */

int mutex_trylock (mutex_id mutex)
    {
    return mutex_timedlock (mutex, NO_WAIT);
    }

int __mutex_unlock (uintptr_t arg1, uintptr_t arg2)
    {
    mutex_id mutex = (mutex_id) arg1;
    task_id  task;

    (void) arg2;

    WARN_ON (mutex->owner != current,
             errno = ERRNO_MUTEX_ILLEGAL_OPERATION; return -1,
             "Invalid operation!");

    if (--mutex->recurse != 0)
        {
        return 0;
        }

    /* remove mutex form current->mutex_owned */

    dlist_del (&mutex->node);

    (void) __recalc_task_prio (current);

    if (dlist_empty (&mutex->pend_q))
        {
        mutex->owner = NULL;
        return 0;
        }

    task = container_of (mutex->pend_q.next, task_t, pq_node);

    task->wanted.id = NULL;

    /* task->c_prio needless change */

    task_ready_q_add (task);

    __mutex_set_owner (mutex, task);

    return 0;
    }

/**
 * mutex_unlock - unlock a mutex
 * @mutex: the mutex to be unlocked
 *
 * return: 0 on success, negtive value on error
 */

int mutex_unlock (mutex_id mutex)
    {
    int ret;

    WARN_ON (mutex == NULL,
             errno = ERRNO_MUTEX_ILLEGAL_ID; return -1,
             "Invalid mutex id");

    if (unlikely (current == NULL))
        {
        return 0;       /* pre-kernel, no racing */
        }

    if (unlikely (obj_verify_protect (mutex_class, &mutex->obj) != 0))
        {
        return -1;
        }

    ret = do_critical_non_irq (__mutex_unlock, (uintptr_t) mutex, 0);

    obj_unprotect (&mutex->obj);

    return ret;
    }

static int __mutex_init (obj_id obj, va_list valist)
    {
    mutex_id mutex = container_of (obj, mutex_t, obj);

    mutex->recurse = 0;
    mutex->owner   = NULL;

    dlist_init (&mutex->pend_q);

    return 0;
    }

static int __mutex_destroy_critical (uintptr_t arg1, uintptr_t arg2)
    {
    mutex_id  mutex = (mutex_id) arg1;
    dlist_t * itr, * n;
    task_id   task;

    (void) arg2;

    /*
     * we must come here from obj_destroy and object already destroyed
     * we do not need to and we can not verify and protect the object
     */

    /* remove this mutex from the list of current->mutex_owned */

    dlist_del (&mutex->node);

    /* task_resume will update the return value to -1 */

    dlist_foreach_safe (itr, n, &mutex->pend_q)
        {
        task = container_of (itr, task_t, pq_node);

        task->wanted.id = NULL;

        /* set the errno of the task */

        task_resume    (task);

        /* set the errno of the task */

        task_errno_set (task, ERRNO_MUTEX_DELETED);
        }

    (void) __recalc_task_prio (mutex->owner);

    return 0;
    }

static int __mutex_destroy (obj_id obj)
    {

    /* this routine is always invoked under task context */

    return do_critical (__mutex_destroy_critical,
                        (uintptr_t) container_of (obj, mutex_t, obj), 0);
    }

/**
 * mutex_lib_init - mutex library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int mutex_lib_init (void)
    {
    BUG_ON (class_init (mutex_class, MID_MUTEX, sizeof (mutex_t),
                        __mutex_init, __mutex_destroy, NULL, NULL) != 0,
            "Fail to initialize mutex_class!");

    return 0;
    }

/*
 * all other kernel module initialization depends on mutex:
 *     xxx_lib_init -> class_init -> mutex_init
 * initialize mutex module in prekernel group
 */

MODULE_INIT (prekernel, mutex_lib_init);

#ifdef CONFIG_SYSCALL
const uintptr_t syscall_entries_mutex [] =
    {
    (uintptr_t) mutex_create,
    (uintptr_t) mutex_open,
    (uintptr_t) mutex_destroy,
    (uintptr_t) mutex_timedlock,
    (uintptr_t) mutex_lock,
    (uintptr_t) mutex_trylock,
    (uintptr_t) mutex_unlock,
    };

const struct syscall_table syscall_table_mutex =
    {
    ARRAY_SIZE (syscall_entries_mutex),
    syscall_entries_mutex
    };
#endif

