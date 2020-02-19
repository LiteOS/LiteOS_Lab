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

/* typedefs */

struct message
{
    struct message *next;
    void           *buff;
};

struct service
{
    const char     *name;
    struct service *next;
    void           *task;
    osal_semp_t     sem;
    struct message *head;
    struct message *tail;
    int           (*handler) (void *);
    bool_t          exit;
    int             stack_size;
    int             prio;
};

/* locals */

static struct service *__services = NULL;
static struct message *__messages = NULL;
static osal_mutex_t    __services_lock;

static void __msg_grow (void)
{
    int i;

    if (__messages != NULL)
    {
        return;
    }

    __messages = (struct message *) osal_malloc (NR_GROW_MESSAGES * sizeof (struct message));

    if (__messages == NULL)
    {
        return;
    }

    for (i = 0; i < NR_GROW_MESSAGES - 1; i++)
    {
        __messages [i].next = __messages + i + 1;
    }

    __messages [i].next = NULL;
}

static struct message *__get_message (void)
{
    struct message *message = NULL;

    if (osal_mutex_lock (__services_lock))
    {
        __msg_grow ();

        if (__messages != NULL)
        {
            message    = __messages;
            __messages = __messages->next;
        }

        (void) osal_mutex_unlock (__services_lock);
    }

    return message;
}

static void __put_message (struct message *message)
{
    if (osal_mutex_lock (__services_lock))
    {
        message->next = __messages;
        __messages    = message;

        (void) osal_mutex_unlock (__services_lock);
    }
    else
    {
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

    if (osal_mutex_lock (__services_lock))
    {
        itr = __services;

        while (itr != NULL)
        {
            if (strcmp (itr->name, name) == 0)
            {
                service = itr;
                break;
            }

            itr = itr->next;
        }

        (void) osal_mutex_unlock (__services_lock);
    }

    return (service_id) service;
}

/**
 * service_send - send message to a service
 * @sid: the service id
 * @msg: the message address
 *
 * return: the true on success, false on fail
 */

bool_t service_send (service_id sid, void *msg)
{
    struct service *service = (struct service *) sid;
    struct message *message;
    bool_t          ret = false;

    if (service == NULL)
    {
        return ret;
    }

    message = __get_message ();

    if (message == NULL)
    {
        return ret;
    }

    message->buff = msg;
    message->next = NULL;

    if (!osal_mutex_lock (__services_lock))
    {
        __put_message (message);

        return ret;
    }

    if (service->task != NULL)
    {
        if (service->head == NULL)
        {
            service->head = message;
            service->tail = message;
        }
        else
        {
            service->tail->next = message;
        }

        ret = osal_semp_post (service->sem);
    }

    (void) osal_mutex_unlock (__services_lock);

    if (ret == false)
    {
        __put_message (message);
    }

    return ret;
}

static int __service_entry (void *arg)
{
    struct service *service = (struct service *) arg;
    struct message *message;

    while (1)
    {
        if (osal_semp_pend (service->sem, cn_osal_timeout_forever))
        {
            if (service->exit)
            {
                return 0;
            }

            if (osal_mutex_lock (__services_lock))
            {
                message       = service->head;
                service->head = service->head->next;

                (void) osal_mutex_unlock (__services_lock);

                service->handler (message->buff);

                __put_message (message);
            }
        }
    }
}

/**
 * service_start - start a service
 * @sid: the service id
 *
 * return: true if successfull, false otherwise
 */

bool_t service_start (service_id sid)
{
    struct service *service = (struct service *) sid;

    if (service == NULL)
    {
        return false;
    }

    if (!osal_mutex_lock (__services_lock))
    {
        return false;
    }

    if (service->task != NULL)
    {
        goto exit;
    }

    if (!osal_semp_create (&service->sem, NR_MAX_MSGS, 0))
    {
        goto exit;
    }
    
    service->task = osal_task_create (service->name, __service_entry,
                                      (void *) service, service->stack_size,
                                      NULL, service->prio);
    if (service->task == NULL)
    {
        (void) osal_semp_del (service->sem);
    }

exit:
    (void) osal_mutex_unlock (__services_lock);

    return service->task != NULL;
}

/**
 * service_stop - stop a service
 * @sid: the service id
 *
 * return: true if successfull, false otherwise
 */

bool_t service_stop (service_id sid)
{
    struct service *service = (struct service *) sid;

    if (service == NULL)
    {
        return false;
    }

    if (!osal_mutex_lock (__services_lock))
    {
        return false;
    }

    if (service->task != NULL)
    {
        osal_task_kill (service->task);
        osal_semp_del (service->sem);
        service->task = NULL;
    }

    (void) osal_mutex_unlock (__services_lock);

    return true;
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

service_id service_create (const char * name, int (* handler) (void *),
                           int stack_size, int prio)
{
    struct service *service;
    void           *task;

    service = (struct service *) osal_malloc (sizeof (struct service));

    if (service == NULL)
    {
        goto err;
    }

    if (!osal_semp_create (&service->sem, NR_MAX_MSGS, 0))
    {
        goto err_free_service;
    }

    service->handler    = handler;
    service->head       = NULL;
    service->tail       = NULL;
    service->name       = name;
    service->task       = task;
    service->exit       = false;
    service->stack_size = stack_size;
    service->prio       = prio;

    if (!service_start ((service_id) service))
        {
        goto err_free_sem;
        }

    if (!osal_mutex_lock (__services_lock))
        {
        goto err_free_task;
        }

    service->next = __services;

    __services = service;

    (void) osal_mutex_unlock (__services_lock);

    return (service_id) service;

err_free_task:
    osal_task_kill (task);
err_free_sem:
    osal_semp_del (service->sem);
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
    return osal_mutex_create (&__services_lock);
}

#if 1
#include <stdbool.h>

#include <stdio.h>

static volatile bool __exit = false;

static int __servcie_sender (void * arg)
    {
    service_id sid = service_open ("test");
    int        msg = 0;

    (void) arg;

    if (sid == INVALID_SID)
    {
        printf ("Fail to open service!\n");

        return -1;
    }

    while (!__exit)
        {
        service_send (sid, (void *) &msg);
        osal_task_sleep (1000);
        msg++;
        }

    return 0;
    }

static int __test_service_handler (void * arg)
{
    printf ("in service handler, we got: %d\n\r", *((int *) arg));

    return 0;
}

int test_service (void)
{
    if (service_create ("test", __test_service_handler, 0x1000, 10) == INVALID_SID)
    {
        printf ("Fail to create service!\n");

        return -1;
    }

    if (osal_task_create ("sender", __servcie_sender, 0, 0x1000, NULL, 30) == NULL)
    {
        printf ("Fail to create sender!\n");

        return -1;
    }

    return 0;
}

#endif