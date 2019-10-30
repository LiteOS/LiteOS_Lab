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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include <common.h>
#include <class.h>
#include <sem.h>
#include <task.h>
#include <critical.h>
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

class_t sem_class [1];

/**
 * sem_init - initialize a semahpore
 * @sem:   the semaphore to be initialized
 * @value: the initialized semaphore value
 *
 * return: 0 on success, negtive value on error
 */

int sem_init (sem_t * sem, unsigned int value)
    {
    WARN_ON (sem == NULL,
             errno = ERRNO_SEM_ILLEGAL_ID; return -1,
             "Invalid sem id!");

    return obj_init (sem_class, &sem->obj, value);
    }

/**
 * sem_create - create a semahpore
 * @value: the initialized semaphore value
 *
 * return: the allocated semaphore or NULL on error
 */

sem_id sem_create (unsigned int value)
    {

    /*
     * obj is just the first member of semaphore, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj_create (sem_class, value), sem_t, obj);
    }

/**
 * sem_open - open a semahpore with a given name
 * @name:  the name of the semaphore
 * @oflag: flag, 0 or O_CREAT or O_CREAT | O_EXCL
 * @mode:  if oflag has O_CREAT option
 * @value: if oflag has O_CREAT option
 *
 * return: the semaphore id or NULL on error
 */

sem_id sem_open (const char * name, int oflag, ...)
    {
    obj_id   obj;
    va_list valist;

    va_start (valist, oflag);

    obj = obj_open (sem_class, name, oflag, valist);

    va_end (valist);

    /*
     * obj is just the first member of semaphore, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj, sem_t, obj);
    }

/**
 * sem_destroy - destroy a semaphore
 * @sem: the semaphore to be destroied
 *
 * return: 0 on success, negtive value on error
 */

int sem_destroy (sem_t * sem)
    {
    WARN_ON (sem == NULL,
             errno = ERRNO_SEM_ILLEGAL_ID; return -1,
             "Invalid sem id!");

    return obj_destroy (sem_class, &sem->obj);
    }

static void __tick_q_callback_sem (task_id task)
    {
    task->wanted.id = NULL;

    /* set the errno of the task */

    task_errno_set (task, ERRNO_SEM_TIMEOUT);
    }

int __sem_wait (uintptr_t arg1, uintptr_t arg2)
    {
    sem_t      * sem     = (sem_t *) arg1;
    unsigned int timeout = (unsigned int) arg2;

    if (atomic_uint_dec_nz (&sem->count))
        {
        return 0;
        }

    current->wanted.id = &sem->obj;

    task_fwait_q_add (&sem->pend_q, timeout, __tick_q_callback_sem);

    return 0;
    }

/**
 * sem_timedwait - lock a semaphore with timeout
 * @sem:     the semaphore to be locked
 * @timeout: the max number of waiting ticks
 *
 * return: 0 on success, negtive value on error
 */

int sem_timedwait (sem_t * sem, unsigned int timeout)
    {
    int ret = 0;

    WARN_ON (sem == NULL,
             errno = ERRNO_SEM_ILLEGAL_ID; return -1,
             "Invalid sem id");

    if (unlikely (obj_verify_protect (sem_class, &sem->obj) != 0))
        {
        return -1;
        }

    if (unlikely (!atomic_uint_dec_nz (&sem->count)))
        {
        if (likely (timeout != 0))
            {
            ret = do_critical_might_sleep (__sem_wait, (uintptr_t) sem,
                                           (uintptr_t) timeout);
            }
        else
            {
            errno = ERRNO_SEM_UNAVAILABLE;
            ret   = -1;
            }
        }

    obj_unprotect (&sem->obj);

    return ret;
    }

/**
 * sem_wait - lock a semaphore
 * @sem: the semaphore to be locked
 *
 * return: 0 on success, negtive value on error
 */

int sem_wait (sem_t * sem)
    {
    return sem_timedwait (sem, WAIT_FOREVER);
    }

/**
 * sem_trywait - try to lock a semaphore
 * @sem: the semaphore to be locked
 *
 * return: 0 on success, negtive value on error
 */

int sem_trywait (sem_t * sem)
    {
    return sem_timedwait (sem, NO_WAIT);
    }

int __sem_post (uintptr_t arg1, uintptr_t arg2)
    {
    sem_t       * sem = (sem_t *) arg1;
    struct task * task;

    (void) arg2;

    if (dlist_empty (&sem->pend_q))
        {
        if (unlikely (!atomic_uint_add_ne (&sem->count, UINT_MAX, 1)))
            {
            errno = ERRNO_SEM_OVERFLOW;
            return -1;      /* overflow */
            }
        }
    else
        {
        task = container_of (sem->pend_q.next, struct task, pq_node);
        task->wanted.id = NULL;
        task_ready_q_add (task);
        }

    return 0;
    }

/**
 * sem_post - unlock a semaphore
 * @sem: the semaphore to be unlocked
 *
 * return: 0 on success, negtive value on error
 */

int sem_post (sem_t * sem)
    {
    int ret;

    WARN_ON (sem == NULL,
             errno = ERRNO_SEM_ILLEGAL_ID; return -1,
             "Invalid sem id!");

    if (unlikely (obj_verify_protect (sem_class, &sem->obj) != 0))
        {
        return -1;
        }

    ret = do_critical (__sem_post, (uintptr_t) sem, 0);

    obj_unprotect (&sem->obj);

    return ret;
    }

/**
 * sem_getvalue - get the value of a semaphore
 * @sem: the semaphore
 *
 * return: 0 on success, negtive value on error
 */

unsigned int sem_getvalue (sem_t * sem)
    {
    return atomic_uint_get (&sem->count);
    }

static int __sem_init (obj_id obj, va_list valist)
    {
    sem_id sem = container_of (obj, sem_t, obj);

    atomic_uint_set (&sem->count, va_arg (valist, unsigned int));

    dlist_init (&sem->pend_q);

    return 0;
    }

static int __sem_destroy_critical (uintptr_t arg1, uintptr_t arg2)
    {
    sem_t   * sem = (sem_t *) arg1;
    dlist_t * itr, * n;

    (void) arg2;

    /*
     * we must come here from obj_destroy and object already destroyed
     * we do not need to and we can not verify and protect the object
     */

    /* task_resume will update the return value to -1 */

    dlist_foreach_safe (itr, n, &sem->pend_q)
        {
        task_id task = container_of (itr, task_t, pq_node);

        task_resume    (task);

        /* set the errno of the task */

        task_errno_set (task, ERRNO_SEM_DELETED);
        }

    return 0;
    }

static int __sem_destroy (obj_id obj)
    {

    /* this routine is always invoked under task context */

    return do_critical (__sem_destroy_critical,
                        (uintptr_t) container_of (obj, sem_t, obj), 0);
    }

/**
 * sem_lib_init - semaphore library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int sem_lib_init (void)
    {
    BUG_ON (class_init (sem_class, MID_SEM, sizeof (sem_t),
                        __sem_init, __sem_destroy, NULL, NULL) != 0,
            "Fail to init sem_class!");

    return 0;
    }

MODULE_INIT (kernel, sem_lib_init);

#ifdef CONFIG_SYSCALL
static const uintptr_t syscall_entries_sem [] =
    {
    (uintptr_t) sem_create,
    (uintptr_t) sem_open,
    (uintptr_t) sem_destroy,
    (uintptr_t) sem_timedwait,
    (uintptr_t) sem_wait,
    (uintptr_t) sem_trywait,
    (uintptr_t) sem_post,
    (uintptr_t) sem_getvalue,
    };

const struct syscall_table syscall_table_sem =
    {
    ARRAY_SIZE (syscall_entries_sem),
    syscall_entries_sem
    };
#endif

