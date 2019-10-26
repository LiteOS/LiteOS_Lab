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

#include <lwip/debug.h>
#include <lwip/def.h>
#include <lwip/sys.h>
#include <lwip/mem.h>
#include <lwip/stats.h>
#include <lwip/tcpip.h>

#include <task.h>
#include <mutex.h>
#include <sem.h>
#include <msg_queue.h>
#include <bug.h>
#include <init.h>

/* macros */

#define MS_PER_SECOND       1000

/* locals */

static mutex_t __prot_lock;

static inline u32_t __ms_to_tick (u32_t ms)
    {
    static const unsigned int ms_per_tick = MS_PER_SECOND / CONFIG_SYS_TICK_HZ;

    return (ms + ms_per_tick - 1) / ms_per_tick;
    }

void sys_init (void)
    {
    BUG_ON (mutex_init (&__prot_lock) != 0, "Fail to initialize __prot_lock!");
    }

u32_t sys_now (void)
    {
    return tick_count_get () * CONFIG_SYS_TICK_HZ;
    }

u32_t sys_jiffies (void)
    {
    return tick_count_get ();
    }

sys_prot_t sys_arch_protect (void)
    {
    mutex_lock (&__prot_lock);
    return 0;
    }

void sys_arch_unprotect (sys_prot_t pval)
    {
    mutex_unlock (&__prot_lock);
    }

void sys_arch_msleep (u32_t delay_ms)
    {
    task_delay (__ms_to_tick (delay_ms));
    }

err_t sys_mutex_new (sys_mutex_t * mutex)
    {
    LWIP_ASSERT ("mutex != NULL", mutex != NULL);

    mutex->mut = (void *) mutex_create ();

    if(mutex->mut == NULL)
        {
        SYS_STATS_INC (mutex.err);
        return ERR_MEM;
        }

    SYS_STATS_INC_USED (mutex);

    return ERR_OK;
    }

void sys_mutex_lock (sys_mutex_t * mutex)
    {
    int ret;

    LWIP_ASSERT ("mutex != NULL", mutex != NULL);
    LWIP_ASSERT ("mutex->mut != NULL", mutex->mut != NULL);

    ret = mutex_lock (mutex->mut);

    LWIP_ASSERT ("failed to take the mutex", ret == 0);
    }

void sys_mutex_unlock (sys_mutex_t * mutex)
    {
    int ret;

    LWIP_ASSERT ("mutex != NULL", mutex != NULL);
    LWIP_ASSERT ("mutex->mut != NULL", mutex->mut != NULL);

    ret = mutex_unlock (mutex->mut);

    LWIP_ASSERT ("failed to give the mutex", ret == 0);
    }

void sys_mutex_free (sys_mutex_t * mutex)
    {
    LWIP_ASSERT ("mutex != NULL", mutex != NULL);
    LWIP_ASSERT ("mutex->mut != NULL", mutex->mut != NULL);

    SYS_STATS_DEC (mutex.used);

    mutex_destroy (mutex->mut);

    mutex->mut = NULL;
    }

err_t sys_sem_new (sys_sem_t * sem, u8_t initial_count)
    {
    LWIP_ASSERT ("sem != NULL", sem != NULL);

    sem->sem = (void *) sem_create (initial_count);

    if(sem->sem == NULL)
        {
        SYS_STATS_INC (sem.err);
        return ERR_MEM;
        }

    SYS_STATS_INC_USED (sem);

    return ERR_OK;
    }

void sys_sem_signal (sys_sem_t *sem)
    {
    int ret;

    LWIP_ASSERT ("sem != NULL", sem != NULL);
    LWIP_ASSERT ("sem->sem != NULL", sem->sem != NULL);

    ret = sem_post ((sem_id) sem->sem);

    /* queue full is OK, this is a signal only... */

    LWIP_ASSERT ("sys_sem_signal: sane return value",
                 (ret == 0) || (errno == ERRNO_SEM_OVERFLOW));
    }

u32_t sys_arch_sem_wait (sys_sem_t * sem, u32_t timeout_ms)
    {
    int ret;

    LWIP_ASSERT ("sem != NULL", sem != NULL);
    LWIP_ASSERT ("sem->sem != NULL", sem->sem != NULL);

    if (timeout_ms == 0)
        {
        ret = sem_wait ((sem_id) sem->sem);
        }
    else
        {
        ret = sem_timedwait ((sem_id) sem->sem, __ms_to_tick (timeout_ms));
        }

    if ((ret != 0) && (errno == ERRNO_SEM_TIMEOUT))
        {
        return SYS_ARCH_TIMEOUT;
        }

    LWIP_ASSERT ("sys_sem_signal: sane return value", ret == 0);

    return 1;
    }

void sys_sem_free (sys_sem_t * sem)
    {
    LWIP_ASSERT ("sem != NULL", sem != NULL);
    LWIP_ASSERT ("sem->sem != NULL", sem->sem != NULL);

    SYS_STATS_DEC (sem.used);

    sem_destroy (sem->sem);

    sem->sem = NULL;
    }

err_t sys_mbox_new (sys_mbox_t * mbox, int size)
    {
    LWIP_ASSERT ("mbox != NULL", mbox != NULL);
    LWIP_ASSERT ("size > 0", size > 0);

    mbox->mbox = mq_create (sizeof (void *), (size_t) size, 0);

    if (mbox->mbox == NULL)
        {
        SYS_STATS_INC (mbox.err);
        return ERR_MEM;
        }

    SYS_STATS_INC_USED (mbox);

    return ERR_OK;
    }

void sys_mbox_post (sys_mbox_t * mbox, void * msg)
    {
    int ret;

    LWIP_ASSERT ("mbox != NULL", mbox != NULL);
    LWIP_ASSERT ("mbox->mbox != NULL", mbox->mbox != NULL);

    ret = mq_send (mbox->mbox, &msg, sizeof (void *));

    LWIP_ASSERT ("mbox post failed", ret == 0);
    }

err_t sys_mbox_trypost (sys_mbox_t * mbox, void * msg)
    {
    int ret;

    LWIP_ASSERT ("mbox != NULL", mbox != NULL);
    LWIP_ASSERT ("mbox->mbox != NULL", mbox->mbox != NULL);

    ret = mq_trysend (mbox->mbox, &msg, sizeof (void *));

    if (ret == 0)
        {
        return ERR_OK;
        }
    else
        {
        LWIP_ASSERT("mbox trypost failed", errno == ERRNO_MQ_UNAVAILABLE);
        SYS_STATS_INC (mbox.err);
        return ERR_MEM;
        }
    }

err_t sys_mbox_trypost_fromisr (sys_mbox_t * mbox, void * msg)
    {
    return sys_mbox_trypost (mbox, msg);
    }

u32_t sys_arch_mbox_fetch (sys_mbox_t * mbox, void ** msg, u32_t timeout_ms)
    {
    int    ret;
    void * msg_dummy;

    LWIP_ASSERT ("mbox != NULL", mbox != NULL);
    LWIP_ASSERT ("mbox->mbox != NULL", mbox->mbox != NULL);

    if (msg == NULL)
        {
        msg = &msg_dummy;
        }

    if (timeout_ms == 0)
        {
        ret = mq_recv (mbox->mbox, msg, sizeof (void *));
        }
    else
        {
        ret = mq_timedrecv (mbox->mbox, msg, sizeof (void *), __ms_to_tick (timeout_ms));

        if ((ret != 0) && (errno == ERRNO_MQ_TIMEOUT))
            {
            *msg = NULL;
            return SYS_ARCH_TIMEOUT;
            }
        }

    LWIP_ASSERT ("mbox fetch failed", ret == 0);

    return 1;
    }

u32_t sys_arch_mbox_tryfetch (sys_mbox_t * mbox, void ** msg)
    {
    int    ret;
    void * msg_dummy;

    LWIP_ASSERT ("mbox != NULL", mbox != NULL);
    LWIP_ASSERT ("mbox->mbox != NULL", mbox->mbox != NULL);

    if (msg == NULL)
        {
        msg = &msg_dummy;
        }

    ret = mq_tryrecv (mbox->mbox, msg, sizeof (void *));

    if ((ret != 0) && (errno == ERRNO_MQ_UNAVAILABLE))
        {
        *msg = NULL;
        return SYS_MBOX_EMPTY;
        }

    LWIP_ASSERT ("mbox fetch failed", ret == 0);

    return 1;
    }

void sys_mbox_free (sys_mbox_t * mbox)
    {
    LWIP_ASSERT ("mbox != NULL", mbox != NULL);
    LWIP_ASSERT ("mbox->mbox != NULL", mbox->mbox != NULL);

    mq_delete (mbox->mbox);

    SYS_STATS_DEC (mbox.used);

    mbox->mbox = NULL;
    }

sys_thread_t sys_thread_new (const char * name, lwip_thread_fn thread, void * arg,
                             int stacksize, int prio)
    {
    task_id      task;
    sys_thread_t lwip_thread;

    LWIP_ASSERT ("invalid stacksize", stacksize > 0);
    LWIP_ASSERT ("invalid stacksize", prio > 0);
    LWIP_ASSERT ("invalid stacksize", prio < TASK_PRIO_INV);

    task = task_spawn (name, (uint8_t) prio, 0, (size_t) stacksize,
                       (int (*) (uintptr_t)) thread, (uintptr_t) arg);

    LWIP_ASSERT ("task creation failed", task != NULL);

    lwip_thread.thread = (void *) task;

    return lwip_thread;
    }

static int lwip_lib_init (void)
    {
    tcpip_init (NULL, NULL);

    return 0;
    }

MODULE_INIT (postkernel, lwip_lib_init);
