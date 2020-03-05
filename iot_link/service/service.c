/*----------------------------------------------------------------------------
 * Copyright (c) <2020>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <osal.h>

#include "service.h"

/* macros */

#define NR_GROW_MESSAGES            16
#define NR_MAX_MSGS                 65535
#define SERVICE_TASK_NAME           "service"
#define SERVICE_TASK_PRIO           1
#define SERVICE_TASK_STACK_SIZE     0x1000

/* typedefs */

struct message {
    struct message *next;
    struct service *service;
    void           *buff;
    void          (*pfn) (void *, int);
};

struct service {
    const char     *name;
    struct service *next;
    int           (*handler) (void *);
    int             prio;
    int             open_count;
    bool_t          running;
    uint64_t        recieved;
    uint64_t        dropped;
    uint64_t        handled;
};

/* locals */

static struct service   *__services     = NULL;
static struct message   *__message_pool = NULL;
static struct message   *__message_heads [32];
static struct message   *__message_tails [32];
static osal_mutex_t      __services_lock;
static osal_semp_t       __services_sem;
static volatile uint32_t __services_bitmap = 0;
static void             *__services_task;

static void __msg_grow (void)
{
    int i;

    if (__message_pool != NULL) {
        return;
    }

    __message_pool = (struct message *) osal_malloc (NR_GROW_MESSAGES * sizeof (struct message));

    if (__message_pool == NULL) {
        return;
    }

    for (i = 0; i < NR_GROW_MESSAGES - 1; i++) {
        __message_pool [i].next = __message_pool + i + 1;
    }

    __message_pool [i].next = NULL;
}

static struct message *__get_message (void)
{
    struct message *message = NULL;

    if (osal_mutex_lock (__services_lock)) {
        __msg_grow ();

        if (__message_pool != NULL) {
            message        = __message_pool;
            __message_pool = __message_pool->next;
        }

        (void) osal_mutex_unlock (__services_lock);
    }

    return message;
}

static void __put_message (struct message *message)
{
    if (osal_mutex_lock (__services_lock)) {
        message->next  = __message_pool;
        __message_pool = message;

        (void) osal_mutex_unlock (__services_lock);
    } else {
        // we should never be here
    }
}

/**
 * service_open - open a service
 * @name: the service name
 *
 * return: the service id or 0 on error
 */

service_id service_open (const char *name)
{
    struct service *service = NULL;
    struct service *itr;

    if (osal_mutex_lock (__services_lock)) {
        itr = __services;

        while (itr != NULL) {
            if (strcmp (itr->name, name) == 0) {
                service = itr;
                service->open_count++;
                break;
            }

            itr = itr->next;
        }

        (void) osal_mutex_unlock (__services_lock);
    }

    return (service_id) service;
}

/**
 * service_close - close a service
 * @sid: the service id
 *
 * return: the true on success, false on fail
 */

bool_t service_close (service_id sid)
{
    struct service *service = (struct service *) sid;

    if (service == NULL) {
        return false;
    }

    if (!osal_mutex_lock (__services_lock)) {
        return false;
    }

    if (service->open_count > 0) {
        service->open_count--;
    }

    (void) osal_mutex_unlock (__services_lock);

    return true;
}

/**
 * service_send - send message to a service
 * @sid: the service id
 * @msg: the message address
 * @pfn: the callback after the message is handled
 *
 * return: the true on success, false on fail
 */

bool_t service_send (service_id sid, void *msg, void (*pfn) (void *, int))
{
    struct service *service = (struct service *) sid;
    struct message *message;

    if (service == NULL) {
        return false;
    }

    if (service->prio >= 32) {
        return false;
    }

    message = __get_message ();

    if (message == NULL) {
        return false;
    }

    message->buff    = msg;
    message->pfn     = pfn;
    message->next    = NULL;
    message->service = service;

    if (!osal_mutex_lock (__services_lock)) {
        __put_message (message);

        return false;
    }

    if (service->running) {
        if (__message_heads [service->prio] == NULL) {
            __message_heads [service->prio] = message;
            __message_tails [service->prio] = message;
        } else {
            __message_tails [service->prio]->next = message;
            __message_tails [service->prio] = message;
        }

        service->recieved++;

        __services_bitmap |= 1 << service->prio;

        (void) osal_semp_post (__services_sem);
    } else {
        service->dropped++;
    }

    (void) osal_mutex_unlock (__services_lock);

    return true;
}

// x will never be 0
static int __find_first_bit (uint32_t x)
{
    int n = 1;

    if ((x & 0x0000ffff) == 0) { n = n + 16; x = x >> 16; }
    if ((x & 0x000000ff) == 0) { n = n +  8; x = x >>  8; }
    if ((x & 0x0000000f) == 0) { n = n +  4; x = x >>  4; }
    if ((x & 0x00000003) == 0) { n = n +  2; x = x >>  2; }

    return n - (x & 1);
}

// process a highest priority service every time
static void __service_process (void)
{
    struct service *service;
    struct message *message;
    int             ret;
    int             i = __find_first_bit (__services_bitmap);

    while (!osal_mutex_lock (__services_lock)) {
        printf ("Fail to get __services_lock, retry!\n\r");
    }

    message = __message_heads [i];

    if (message != NULL) {
        if (message->service->running) {
            service = message->service;

            ret = service->handler (message->buff);

            if (message->pfn != NULL) {
                message->pfn (message->buff, ret);
            }

            service->handled++;
        } else {
            service->dropped++;
        }

        __message_heads [i] = message->next;
    } else {
        __services_bitmap &= ~(1 << i);
    }

    (void) osal_mutex_unlock (__services_lock);

    if (message != NULL) {
        __put_message (message);
    }
}

static int __service_entry (void *arg)
{
    while (1) {
        while (!osal_semp_pend (__services_sem, cn_osal_timeout_forever)) {
            printf ("Fail to pend __services_sem, retry!\n\r");
        }

        while (__services_bitmap != 0) {
            __service_process ();
        }
    }

    return 0;
}

static bool_t __service_run (service_id sid, bool_t running)
{
    struct service *service = (struct service *) sid;

    if (service == NULL) {
        return false;
    }

    if (!osal_mutex_lock (__services_lock)) {
        return false;
    }

    service->running = running;

    (void) osal_mutex_unlock (__services_lock);

    return true;
}

/**
 * service_start - start a service
 * @sid: the service id
 *
 * return: true if successfull, false otherwise
 */

bool_t service_start (service_id sid)
{
    return __service_run (sid, true);
}

/**
 * service_stop - stop a service
 * @sid: the service id
 *
 * return: true if successfull, false otherwise
 */

bool_t service_stop (service_id sid)
{
    return __service_run (sid, false);
}

/**
 * service_restart - restart a service
 * @sid: the service id
 *
 * return: true if successfull, false otherwise
 */

bool_t service_restart (service_id sid)
{
    if (!service_stop (sid))
    {
        return false;
    }

    return service_start (sid);
}

/**
 * service_create - create a service, the service will be started by default
 * @name:       the service name
 * @handler:    the service handler
 * @stack_size: the stack size of the corresponding task
 * @prio:       the priority the of the corresponding task
 *
 * return: valid service id on success, invalid service id otherwise
 */

service_id service_create (const char * name, int (* handler) (void *), int prio)
{
    struct service *service;

    service = (struct service *) osal_malloc (sizeof (struct service));

    if (service == NULL) {
        goto err;
    }

    service->handler    = handler;
    service->name       = name;
    service->prio       = prio;
    service->open_count = 0;
    service->running    = true;
    service->recieved   = 0;
    service->handled    = 0;
    service->dropped    = 0;

    if (!osal_mutex_lock (__services_lock)) {
        goto err_free_service;
    }

    service->next = __services;

    __services = service;

    (void) osal_mutex_unlock (__services_lock);

    return (service_id) service;

err_free_service:
    osal_free (service);
err:
    return INVALID_SID;
    }

/**
 * service_init - service module initialization
 *
 * return: true if successfull, false otherwise
 */

bool_t service_init (void)
{
    if (!osal_semp_create (&__services_sem, NR_MAX_MSGS, 0)) {
        return false;
    }

    if (!osal_mutex_create (&__services_lock)) {
        goto err_free_sem;
    }

    __services_task = osal_task_create (SERVICE_TASK_NAME, __service_entry, 0,
                                        SERVICE_TASK_STACK_SIZE, NULL,
                                        SERVICE_TASK_PRIO);

    if (__services_task == NULL) {
        goto err_free_mutex;
    }

    return true;

err_free_mutex:
    osal_mutex_del (__services_lock);
err_free_sem:
    osal_semp_del (__services_sem);
    return false;
}
