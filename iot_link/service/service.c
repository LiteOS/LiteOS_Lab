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

#define NR_GROW_MESSAGES                    32
#define NR_MAX_MSGS                         65535

#define DEVICE_SERVICE_TASK_NAME            SERVICE_DOMAIN_DEVICE
#define DEVICE_SERVICE_TASK_PRIO            2
#define DEVICE_SERVICE_TASK_STACK_SIZE      0x1000

#define SYSTEM_SERVICE_TASK_NAME            SERVICE_DOMAIN_SYSTEM
#define SYSTEM_SERVICE_TASK_PRIO            5
#define SYSTEM_SERVICE_TASK_STACK_SIZE      0x1000

#define USER_SERVICE_TASK_NAME              SERVICE_DOMAIN_USER
#define USER_SERVICE_TASK_PRIO              10
#define USER_SERVICE_TASK_STACK_SIZE        0x1000

/* typedefs */

struct message {
    struct message         *next;
    struct service         *service;
    void                   *buff;
    void                  (*pfn) (void *, int);
};

struct service {
    const char             *name;
    struct service         *next;
    struct service_manager *manager;
    int                   (*handler) (void *);
    int                     prio;
    int                     open_count;
    bool_t                  running;
    uint64_t                recieved;
    uint64_t                dropped;
    uint64_t                handled;
};

struct service_manager {
    struct service         *services;
    struct message         *message_pool;
    struct message         *message_heads [32];
    struct message         *message_tails [32];
    osal_mutex_t            services_lock;
    osal_semp_t             services_sem;
    volatile uint32_t       services_bitmap;
    void                   *services_task;
    void                  (*msg_pool_grow) (struct service_manager *);
    struct message       *(*msg_pool_get) (struct service_manager *);
    void                  (*msg_pool_put) (struct service_manager *);
    bool_t                (*lock) (struct service_manager *, int *flags);
    void                  (*unlock) (struct service_manager *, int);
};

/* locals */

static struct service_manager __service_manager_device;
static struct service_manager __service_manager_system;
static struct service_manager __service_manager_user;

static void __msg_grow (struct service_manager *manager)
{
    int i;
    int flags;

    if (!manager->lock (manager, &flags)) {
        return;
    }

    if (manager->message_pool == NULL) {
        manager->message_pool = (struct message *) osal_malloc (NR_GROW_MESSAGES * sizeof (struct message));

        if (manager->message_pool != NULL) {
            for (i = 0; i < NR_GROW_MESSAGES - 1; i++) {
                manager->message_pool [i].next = manager->message_pool + i + 1;
            }

        manager->message_pool [i].next = NULL;
        }
    }

    (void) manager->unlock (manager, flags);
}

static struct message *__get_message (struct service_manager *manager)
{
    struct message *message = NULL;
    int             flags;

    if (manager->lock (manager, &flags)) {
        if (manager->message_pool != NULL) {
            message               = manager->message_pool;
            manager->message_pool = manager->message_pool->next;
        }

        (void) manager->unlock (manager, flags);
    }

    return message;
}

static void __put_message (struct service_manager *manager, struct message *message)
{
    int flags;

    if (manager->lock (manager, &flags)) {
        message->next         = manager->message_pool;
        manager->message_pool = message;

        (void) manager->unlock (manager, flags);
    } else {
        // we should never be here
    }
}

static struct service_manager *__get_domain (const char * domain)
{
    if (strcmp (domain, SERVICE_DOMAIN_DEVICE) == 0) {
        return &__service_manager_device;
    }

    if (strcmp (domain, SERVICE_DOMAIN_SYSTEM) == 0) {
        return &__service_manager_system;
    }

    if (strcmp (domain, SERVICE_DOMAIN_USER) == 0) {
        return &__service_manager_user;
    }

    return NULL;
}

/**
 * service_open - open a service
 * @name: the service name
 *
 * return: the service id or 0 on error
 */

service_id service_open (const char * domain, const char *name)
{
    struct service         *service = NULL;
    struct service         *itr;
    struct service_manager *manager;
    int                     flags;

    manager = __get_domain (domain);

    if (manager == NULL) {
        return INVALID_SID;
    }

    if (manager->lock (manager, &flags)) {
        itr = manager->services;

        while (itr != NULL) {
            if (strcmp (itr->name, name) == 0) {
                service = itr;
                service->open_count++;
                break;
            }

            itr = itr->next;
        }

        (void) manager->unlock (manager, flags);
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
    struct service         *service = (struct service *) sid;
    struct service_manager *manager;
    int                     flags;

    if (service == NULL) {
        return false;
    }

    manager = service->manager;

    if (manager == NULL) {
        return false;
    }

    if (!manager->lock (manager, &flags)) {
        return false;
    }

    if (service->open_count > 0) {
        service->open_count--;
    }

    (void) manager->unlock (manager, flags);

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
    struct service         *service = (struct service *) sid;
    struct service_manager *manager;
    struct message         *message;
    int                     flags;
    bool_t                  dropped = false;

    if (service == NULL) {
        return false;
    }

    if (service->prio >= 32) {
        return false;
    }

    manager = service->manager;

    if (manager == NULL) {
        return false;
    }

    __msg_grow (manager);

    message = __get_message (manager);

    if (message == NULL) {
        return false;
    }

    message->buff    = msg;
    message->pfn     = pfn;
    message->next    = NULL;
    message->service = service;

    if (!manager->lock (manager, &flags)) {
        __put_message (manager, message);

        return false;
    }

    if (service->running) {
        if (manager->message_heads [service->prio] == NULL) {
            manager->message_heads [service->prio] = message;
            manager->message_tails [service->prio] = message;
        } else {
            manager->message_tails [service->prio]->next = message;
            manager->message_tails [service->prio] = message;
        }

        service->recieved++;

        manager->services_bitmap |= 1 << service->prio;

        (void) osal_semp_post (manager->services_sem);
    } else {
        service->dropped++;
        dropped = true;
    }

    (void) manager->unlock (manager, flags);

    if (dropped) {
        __put_message (manager, message);
        return false;
    } else {
        return true;
    }
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
static void __service_process (struct service_manager *manager)
{
    struct service *service;
    struct message *message;
    int             flags;
    int             ret;
    // manager->services_bitmap is only cleaned by us, it is safe do this here
    int             i = __find_first_bit (manager->services_bitmap);

    while (!manager->lock (manager, &flags)) {
        printf ("Fail to get __services_lock, retry!\n\r");
    }

    message = manager->message_heads [i];

    if (message != NULL) {
        service = message->service;

        if (service->running) {
            ret = service->handler (message->buff);

            if (message->pfn != NULL) {
                message->pfn (message->buff, ret);
            }

            service->handled++;
        } else {
            service->dropped++;
        }

        manager->message_heads [i] = message->next;
    } else {
        manager->services_bitmap &= ~(1 << i);
    }

    (void) manager->unlock (manager, flags);

    if (message != NULL) {
        __put_message (manager, message);
    }
}

static int __service_entry (void *arg)
{
    struct service_manager *manager = (struct service_manager *) arg;

    while (1) {
        while (!osal_semp_pend (manager->services_sem, cn_osal_timeout_forever)) {
            printf ("Fail to pend __services_sem, retry!\n\r");
        }

        while (manager->services_bitmap != 0) {
            __service_process (manager);
        }
    }

    return 0;
}

static bool_t __service_run (service_id sid, bool_t running)
{
    struct service         *service = (struct service *) sid;
    struct service_manager *manager;
    int                     flags;

    if (service == NULL) {
        return false;
    }

    manager = service->manager;

    if (manager == NULL) {
        return false;
    }

    if (!manager->lock (manager, &flags)) {
        return false;
    }

    service->running = running;

    (void) manager->unlock (manager, flags);

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
    if (!service_stop (sid)) {
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

service_id service_create (const char * domain, const char * name, int (* handler) (void *), int prio)
{
    struct service         *service;
    struct service_manager *manager;
    int                     flags;

    manager = __get_domain (domain);

    if (manager == NULL) {
        return INVALID_SID;
    }

    if (prio >= 32) {
        return INVALID_SID;
    }

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
    service->manager    = manager;

    if (!manager->lock (manager, &flags)) {
        goto err_free_service;
    }

    service->next = manager->services;

    manager->services = service;

    (void) manager->unlock (manager, flags);

    return (service_id) service;

err_free_service:
    osal_free (service);
err:
    return INVALID_SID;
    }

static bool_t __service_manager_init (struct service_manager * manager, 
                                      const char * name, int stack_size, int prio,
                                     bool_t (*lock) (struct service_manager *, int *),
                                     void (*unlock) (struct service_manager *, int))
{
    if (!osal_semp_create (&manager->services_sem, NR_MAX_MSGS, 0)) {
        return false;
    }

    if (manager != &__service_manager_device) {
        if (!osal_mutex_create (&manager->services_lock)) {
            goto err_free_sem;
        }
    }

    manager->services_task = osal_task_create (name, __service_entry, (void *) manager,
                                               stack_size, NULL, prio);

    if (manager->services_task == NULL) {
        goto err_free_mutex;
    }

    manager->lock   = lock;
    manager->unlock = unlock;

    __msg_grow (manager);

    return true;

err_free_mutex:
    osal_mutex_del (manager->services_lock);
err_free_sem:
    osal_semp_del (manager->services_sem);
    return false;
}

static void __service_manager_deinit (struct service_manager *manager)
{
    osal_mutex_del (manager->services_lock);
    osal_semp_del (manager->services_sem);
    osal_task_kill (manager->services_task);
}

static bool_t __device_service_lock (struct service_manager *manager, int *flags)
{
    return osal_int_lock (flags);
}

static void __device_service_unlock (struct service_manager *manager, int flag)
{
    osal_int_restore (flag);
}

static bool_t __service_lock (struct service_manager *manager, int *flags)
{
    return osal_mutex_lock (manager->services_lock);
}

static void __service_unlock (struct service_manager *manager, int flag)
{
    osal_mutex_unlock (manager->services_lock);
}

/**
 * service_init - service module initialization
 *
 * return: true if successfull, false otherwise
 */

bool_t service_init (void)
{
    if (!__service_manager_init (&__service_manager_device, DEVICE_SERVICE_TASK_NAME,
                                 DEVICE_SERVICE_TASK_STACK_SIZE, DEVICE_SERVICE_TASK_PRIO,
                                 __device_service_lock, __device_service_unlock)) {
        return false;
    }

    if (!__service_manager_init (&__service_manager_system, SYSTEM_SERVICE_TASK_NAME,
                                 SYSTEM_SERVICE_TASK_STACK_SIZE, SYSTEM_SERVICE_TASK_PRIO,
                                 __service_lock, __service_unlock)) {
        __service_manager_deinit (&__service_manager_device);
        return false;
    }

    if (!__service_manager_init (&__service_manager_user, USER_SERVICE_TASK_NAME,
                                 USER_SERVICE_TASK_STACK_SIZE, USER_SERVICE_TASK_PRIO,
                                 __service_lock, __service_unlock)) {
        __service_manager_deinit (&__service_manager_device);
        __service_manager_deinit (&__service_manager_system);
        return false;
    }

    return true;
}
