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
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

#include <common.h>
#include <class.h>
#include <msg_queue.h>
#include <errno.h>
#include <irq.h>
#include <init.h>
#include <warn.h>

#ifdef CONFIG_SYSCALL
#include <syscall.h>
#endif

#include <arch/config.h>            /* for ALLOC_ALIGN */

/* globals */

/*
 * classes are defined as an array with only 1 element, so we can use it as an
 * pointer
 */

class_t mq_class [1];

/**
 * mq_create - create a message queue
 * @msg_size: the message size of the queue
 * @max_msgs: the max number of messages in the queue
 * @options:  the options, not used for now
 *
 * return: the message queue id on success, NULL on error
 */

mq_id mq_create (size_t msg_size, size_t max_msgs, unsigned int options)
    {

    /*
     * obj is just the first member of mq, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj_create (mq_class, msg_size, max_msgs, options),
                         mq_t, obj);
    }

/**
 * mq_open - open a message queue with a given name
 * @name:     the name of the queue
 * @oflag:    flag, 0 or O_CREAT or O_CREAT | O_EXCL
 * @mode:     if oflag has O_CREAT option
 * @msg_size: if oflag has O_CREAT option, same as mq_create
 * @max_msgs: if oflag has O_CREAT option, same as mq_create
 * @options:  if oflag has O_CREAT option, same as mq_create
 *
 * return: the mq id or NULL on error
 */

mq_id mq_open (const char * name, int oflag, ...)
    {
    obj_id  obj;
    va_list valist;

    va_start (valist, oflag);

    obj = obj_open (mq_class, name, oflag, valist);

    va_end (valist);

    /*
     * obj is just the first member of mq, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj, mq_t, obj);
    }

/**
 * mq_delete - delete a message queue
 * @mq: the message queue to be deleted
 *
 * return: 0 on success, negtive value on error
 *
 * internal: this routine can not be invoked in critical
 */

int mq_delete (mq_id mq)
    {
    WARN_ON (mq == NULL,
             errno = ERRNO_MQ_ILLEGAL_ID; return -1,
             "Invalid mq id!");

    return obj_destroy (mq_class, &mq->obj);
    }

static inline int __mq_transfer (mq_id mq, void * buff, size_t size,
                                 unsigned int timeout, int op)
    {
    unsigned int idx;
    void       * msg;

    WARN_ON (mq   == NULL,
             errno = ERRNO_MQ_ILLEGAL_ID;   return -1,
             "Invalid mq id!");

    WARN_ON (buff == NULL,
             errno = ERRNO_MQ_ILLEGAL_BUFF; return -1,
             "Invalid buffer!");

    WARN_ON (size == 0,
             errno = ERRNO_MQ_ILLEGAL_SIZE; return -1,
             "Invalid size!");

    /*
     * can not recieve in interrupt context, if the interrupt is just happen
     * between in the atomic_inc and memcpy, the msg will be unpredictable
     */

    if (op == MQ_OP_RD && int_context ())
        {
        WARN ("Can not recieve from msg_queue in ISR!");
        errno = ERRNO_MQ_ILLEGAL_OPERATION;

        return -1;
        }

    if (unlikely (obj_verify_protect (mq_class, &mq->obj) != 0))
        {
        return -1;
        }

    size = min (size, mq->msg_size);

    if (unlikely (sem_timedwait (&mq->sem [op], timeout) != 0))
        {

        /* map semaphore errno to mq errno */

        switch (errno)
            {
            case ERRNO_SEM_TIMEOUT:
                errno = ERRNO_MQ_TIMEOUT;
                break;
            case ERRNO_SEM_UNAVAILABLE:
                errno = ERRNO_MQ_UNAVAILABLE;
                break;
            default:
                break;
            }

        obj_unprotect (&mq->obj);

        return -1;
        }

    idx = atomic_uint_inc (&mq->idx [op]) % mq->max_msgs;

    msg = (void *) (mq->msgs + mq->msg_size * idx);

    if (op == MQ_OP_RD)
        {
        memcpy (buff, msg, size);
        }
    else
        {
        memcpy (msg, buff, size);
        }

    (void) sem_post (&mq->sem [1 - op]);

    obj_unprotect (&mq->obj);

    return 0;
    }

/**
 * mq_timedsend - send a message from to message queue with timeout
 * @mq:   the message queue
 * @buff: the buffer holding the message to send
 * @size: the buffer size
 * @timeout: the max number of waiting ticks
 *
 * return: 0 on success, negtive value on error
 */

int mq_timedsend (mq_id mq, void * buff, size_t size, unsigned int timeout)
    {
    return __mq_transfer (mq, buff, size, timeout, MQ_OP_WT);
    }

/**
 * mq_send - send a message from to message queue
 * @mq:   the message queue
 * @buff: the receive buffer
 * @size: the buffer size
 *
 * return: 0 on success, negtive value on error
 */

int mq_send (mq_id mq, void * buff, size_t size)
    {
    return mq_timedsend (mq, buff, size, WAIT_FOREVER);
    }

/**
 * mq_trysend - try to send a message from to message queue
 * @mq:   the message queue
 * @buff: the buffer holding the message to send
 * @size: the buffer size
 *
 * return: 0 on success, negtive value on error
 */

int mq_trysend (mq_id mq, void * buff, size_t size)
    {
    return mq_timedsend (mq, buff, size, NO_WAIT);
    }

/**
 * mq_timedrecv - receive a message from a message queue with timeout
 * @mq:   the message queue
 * @buff: the receive buffer
 * @size: the buffer size
 * @timeout: the max number of waiting ticks
 *
 * return: 0 on success, negtive value on error
 */

int mq_timedrecv (mq_id mq, void * buff, size_t size, unsigned int timeout)
    {
    return __mq_transfer (mq, buff, size, timeout, MQ_OP_RD);
    }

/**
 * mq_recv - receive a message from a message queue
 * @mq:   the message queue
 * @buff: the receive buffer
 * @size: the buffer size
 *
 * return: 0 on success, negtive value on error
 */

int mq_recv (mq_id mq, void * buff, size_t size)
    {
    return mq_timedrecv (mq, buff, size, WAIT_FOREVER);
    }

/**
 * mq_tryrecv - try to receive a message from a message queue
 * @mq:   the message queue
 * @buff: the receive buffer
 * @size: the buffer size
 *
 * return: 0 on success, negtive value on error
 */

int mq_tryrecv (mq_id mq, void * buff, size_t size)
    {
    return mq_timedrecv (mq, buff, size, NO_WAIT);
    }

static int __mq_init (obj_id obj, va_list valist)
    {
    mq_id        mq = container_of (obj, mq_t, obj);
    size_t       msg_size, max_msgs;
    unsigned int options;

    msg_size = va_arg (valist, size_t);
    max_msgs = va_arg (valist, size_t);
    options  = va_arg (valist, unsigned int);

    (void) options;     // not used for now

    WARN_ON (msg_size == 0,
             errno = ERRNO_MQ_ILLEGAL_SIZE; return -1,
             "Invalid msg_size");

    WARN_ON (max_msgs == 0 || max_msgs > (size_t) INT_MAX,
             errno = ERRNO_MQ_ILLEGAL_MSGS; return -1,
             "Invalid max_msgs!");

    msg_size = round_up (msg_size, ALLOC_ALIGN);

    mq->msgs = (char *) malloc (msg_size * max_msgs);

    if (unlikely (mq->msgs == NULL))
        {
        goto err_out;
        }

    if (unlikely (sem_init (&mq->sem [0], 0) != 0))
        {
        goto err_out_free;
        }

    if (unlikely (sem_init (&mq->sem [1], (unsigned int) max_msgs) != 0))
        {
        goto err_out_sem_del;
        }

    mq->msg_size = msg_size;
    mq->max_msgs = max_msgs;

    atomic_uint_set (&mq->idx [0], 0);
    atomic_uint_set (&mq->idx [1], 0);

    return 0;

err_out_sem_del:
    sem_destroy (&mq->sem [0]);
err_out_free:
    free (mq->msgs);
err_out:
    return -1;
    }

static int __mq_destroy (obj_id obj)
    {
    mq_id mq = container_of (obj, mq_t, obj);

    /* should never fail */

    (void) sem_destroy (&mq->sem [0]);
    (void) sem_destroy (&mq->sem [1]);

    free (mq->msgs);

    return 0;
    }

/**
 * mq_lib_init - message queue library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int mq_lib_init (void)
    {
    if (class_init (mq_class, MID_MQ, sizeof (mq_t),
                    __mq_init, __mq_destroy, NULL, NULL) != 0)
        {
        WARN ("fail to initialize mq_class!");
        return -1;
        }

    return 0;
    }

MODULE_INIT (kernel, mq_lib_init);

#ifdef CONFIG_SYSCALL
static const uintptr_t syscall_entries_mq [] =
    {
    (uintptr_t) mq_create,
    (uintptr_t) mq_open,
    (uintptr_t) mq_delete,
    (uintptr_t) mq_timedsend,
    (uintptr_t) mq_send,
    (uintptr_t) mq_trysend,
    (uintptr_t) mq_timedrecv,
    (uintptr_t) mq_recv,
    (uintptr_t) mq_tryrecv,
    };

const struct syscall_table syscall_table_mq =
    {
    ARRAY_SIZE (syscall_entries_mq),
    syscall_entries_mq
    };
#endif

