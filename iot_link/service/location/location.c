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

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <osal.h>

#include <service.h>

#include "location.h"

/* macros */

#define LOCATION_SERVICE_NAME           "location"
#define LOCATION_SERVICE_PRIO           31

/* typedefs */

struct location_callback {
    struct location_callback *next;
    union {
        void                (*pos_callback) (uintptr_t, struct location *);
        void                (*stt_callback) (uintptr_t, int);
        uintptr_t             callback;
    };
    uintptr_t                 arg;
};

/* locals */

static service_id                __location_sid;

static struct location_callback *__stt_change_notify_head = NULL;
static struct location_callback *__stt_change_notify_tail = NULL;

static struct location_callback *__pos_change_notify_head = NULL;
static struct location_callback *__pos_change_notify_tail = NULL;

static osal_mutex_t              __location_lock;

static void __notify_pos (struct location * location)
{
    struct location_callback *itr = __pos_change_notify_head;

    while (itr != NULL) {
        itr->pos_callback (itr->arg, location);
        itr = itr->next;
    }
}

static void __notify_stt (int status)
{
    struct location_callback *itr = __stt_change_notify_head;

    while (itr != NULL) {
        itr->stt_callback (itr->arg, status);
        itr = itr->next;
    }
}

static int __location_service_handler (void *arg)
{
    struct location_msg *msg = (struct location_msg *) arg;

    switch (msg->cmd) {
        case LOCATION_CMD_UPDATE_STATUS:
            __notify_stt (msg->status);
            break;
        case LOCATION_CMD_UPDATE_POSITION:
            __notify_pos (&msg->location);
            break;
        default:
            return -1;
    }

    return 0;
}

int location_service_init (void)
{
    static bool_t inited = false;

    if (inited) {
        return 0;
    }

    if (!osal_mutex_create (&__location_lock)) {
        return -1;
    }

    __location_sid = service_create (SERVICE_DOMAIN_CORE, LOCATION_SERVICE_NAME,
                                     __location_service_handler, LOCATION_SERVICE_PRIO);

    if (__location_sid == INVALID_SID) {
        osal_mutex_del (__location_lock);
    }

    return 0;
}

bool_t __location_service_listen (uintptr_t callback_pfn, uintptr_t arg,
                                  struct location_callback **head,
                                  struct location_callback **tail)
{
    bool_t                     ret = false;
    struct location_callback * callback;

    if (callback_pfn == 0) {
        return false;
    }

    callback = (struct location_callback *) malloc (sizeof (struct location_callback));

    if (callback == NULL) {
        return ret;
    }

    if (!osal_mutex_lock (__location_lock)) {
        free (callback);
        return ret;
    }

    callback->callback = callback_pfn;
    callback->arg      = arg;
    callback->next     = NULL;

    if (*head == NULL) {
        *head = callback;
    }

    *tail = callback;

    (void) osal_mutex_unlock (__location_lock);

    return true;
}

/**
 * location_service_listen_state - listen on the changing of location state
 * @callback: the callback will be called on the changing of location state
 * @arg:      the argument of the callback
 *
 * return: the true on success, false on fail
 */

bool_t location_service_listen_state (void (*callback) (uintptr_t, int), uintptr_t arg)
{
    return __location_service_listen ((uintptr_t) callback, arg,
                                      &__stt_change_notify_head,
                                      &__stt_change_notify_tail);
}

/**
 * location_service_listen_position - listen on the changing of position
 * @callback: the callback will be called on the changing of position
 * @arg:      the argument of the callback
 *
 * return: the true on success, false on fail
 */

bool_t location_service_listen_position (void (*callback) (uintptr_t, struct location *), uintptr_t arg)
{
    return __location_service_listen ((uintptr_t) callback, arg,
                                      &__pos_change_notify_head,
                                      &__pos_change_notify_tail);
}

// simple test code use a task to fake the location device
#if 1
static void * fake_location_device_task = NULL;

static struct location_msg       __location_msg_stt;
static bool_t                    __location_msg_stt_using = false;
static struct location_msg       __location_msg_pos;
static bool_t                    __location_msg_pos_using = false;
static service_id                __location_sid_open;

static void __location_callback (void *arg, int status)
{
    struct location_msg *msg = (struct location_msg *) arg;
    int                  flags;

    if (osal_int_lock (&flags)) {
        if (msg->cmd == LOCATION_CMD_UPDATE_POSITION) {
            __location_msg_pos_using = false;
        } else {
            __location_msg_stt_using = false;
        }

        (void) osal_int_restore (flags);
    }
}

static void __send_msg (struct location_msg *msg, bool_t *using)
{
    if (*using) {
        return;
    }

    *using = true;

    service_send (__location_sid_open, (void *) msg, __location_callback);
}

static int fake_ldev_task (void * arg)
{
    int             step = 0;

    if (location_service_init () != 0) {
        printf ("fail to start location service!\n\r");
        return -1;
    }

    __location_sid_open = service_open (SERVICE_DOMAIN_CORE, LOCATION_SERVICE_NAME);

    if (__location_sid_open == INVALID_SID) {
        return -1;
    }

    __location_msg_stt.cmd        = LOCATION_CMD_UPDATE_STATUS;
    __location_msg_stt.status     = 0;

    __location_msg_pos.cmd        = LOCATION_CMD_UPDATE_POSITION;
    __location_msg_pos.location.x = 0;
    __location_msg_pos.location.y = 0;

    while (1) {
        if (step & 1) {
            __location_msg_stt.status--;
            __send_msg (&__location_msg_stt, &__location_msg_stt_using);
        } else {
            __location_msg_pos.location.x++;
            __location_msg_pos.location.y++;
            __location_msg_pos.location.y++;
            __send_msg (&__location_msg_pos, &__location_msg_pos_using);
        }

        osal_task_sleep (1000);

        step++;
    }

    return 0;
}

static void __usr_pos_callback (uintptr_t arg, struct location * location)
{
    printf ("new status: %d-%d\n\r", location->x,location->y);
}

static void __usr_stt_callback (uintptr_t arg, int status)
{
    printf ("new status: %d\n\r", status);
}

void location_test (void)
{
    fake_location_device_task = osal_task_create ("ldev", fake_ldev_task,
                                                  NULL, 0x800, NULL, 5);

    if (fake_location_device_task == NULL) {
        printf ("Fail to start the fake location device!\n");
    } else {
        printf ("Fake location device started!\n");
    }

    location_service_listen_position (__usr_pos_callback, 0);

    location_service_listen_state (__usr_stt_callback, 0);

    printf ("listening...\n\r");
}
#endif
