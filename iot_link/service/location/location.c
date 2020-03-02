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

#include "location.h"

/* macros */

#define LOcATION_TASK_NAME                      "location"
#define LOCATION_TASK_STACK_SIZE                0x1000
#define LOCATION_TASK_PRIO                      10

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

static bool_t     (*__location_device_start) (void) = NULL;
static bool_t     (*__location_device_stop)  (void) = NULL;
static osal_mutex_t __location_lock;

static void        *__location_task_id = NULL;
static osal_semp_t  __location_sem;

static osal_queue_t __location_pos_queue;
static osal_queue_t __location_stt_queue;

static struct location_callback *__stt_change_notify_head = NULL;
static struct location_callback *__stt_change_notify_tail = NULL;

static struct location_callback *__pos_change_notify_head = NULL;
static struct location_callback *__pos_change_notify_tail = NULL;

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

static int __location_task (void * arg)
{
    struct location location;
    int             status;
    bool            stop;

    while (1) {
        if (!osal_semp_pend (__location_sem, cn_osal_timeout_forever)) {
            continue;
        }

        do {
            stop = true;

            if (osal_queue_recv (__location_pos_queue, &location,
                                 sizeof (struct location), 0)) {
                __notify_pos (&location);
                stop = false;
            }

            if (osal_queue_recv (__location_stt_queue, &status,
                                 sizeof (int), 0)) {
                __notify_stt (status);
                stop = false;
            }
        } while (!stop);
    }

    return 0;
}

/**
 * location_service_start - start the location service
 *
 * return: the true on success, false on fail
 */

bool_t location_service_start (void)
{
    bool_t ret = false;

    if (__location_device_start == NULL) {
        return ret;
    }

    if (!osal_mutex_lock (__location_lock)) {
        return ret;
    }

    if (__location_task_id == NULL) {
        ret = __location_device_start ();

        if (ret) {
            __location_task_id = osal_task_create (LOcATION_TASK_NAME,
                                                   __location_task, NULL,
                                                   LOCATION_TASK_STACK_SIZE,
                                                   NULL, LOCATION_TASK_PRIO);

            ret = __location_task_id != NULL;

            if (!ret && __location_device_stop != NULL) {
                (void) __location_device_stop ();
            }
        }
    }

    (void) osal_mutex_unlock (__location_lock);

    return ret;
}

/**
 * location_service_stop - stop the location service
 *
 * return: the true on success, false on fail
 */

bool_t location_service_stop (void)
{
    bool_t ret = false;

    if (__location_device_stop == NULL) {
        return ret;
    }

    if (!osal_mutex_lock (__location_lock)) {
        return ret;
    }

    if (__location_task_id != NULL) {
        ret = __location_device_stop ();

        if (ret) {
            (void) osal_task_kill (__location_task_id);
            __location_task_id = NULL;
        }
    }

    (void) osal_mutex_unlock (__location_lock);

    return ret;
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

bool_t location_service_listen_state (void (*callback) (uintptr_t, int),
                                      uintptr_t arg)
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

void location_update_status (int status)
{
    osal_queue_send (__location_stt_queue, &status, sizeof (int), 0);
    osal_semp_post (__location_sem);
}

void location_update_location (struct location * location)
{
    osal_queue_send (__location_pos_queue, location, sizeof (struct location), 0);
    osal_semp_post (__location_sem);
}

/**
 * location_service_register - register device to the service
 * @start: routine to start the location device
 * @stop:  routine to stop the location device
 *
 * return: the true on success, false on fail
 */

bool_t location_service_config (bool_t (*start) (void), bool_t (*stop) (void))
{
    if (__location_device_start != NULL || start == NULL) {
        return false;
    }

    if (!osal_mutex_create (&__location_lock)) {
        return false;
    }

    if (!osal_semp_create (&__location_sem, 65535, 0)) {
        goto err_del_mutex;
    }

    if (!osal_queue_create (&__location_pos_queue, 128, sizeof (struct location))) {
        goto err_del_sem;
    }

    if (!osal_queue_create (&__location_stt_queue, 128, sizeof (int))) {
        goto err_del_queue;
    }

    __location_device_start = start;
    __location_device_stop  = stop;

    return true;

err_del_queue:
    (void) osal_queue_del (__location_pos_queue);
err_del_sem:
    (void) osal_semp_del (__location_sem);
err_del_mutex:
    (void) osal_mutex_del (__location_lock);
    return false;
}

#if 1
static void * fake_location_device_task = NULL;

static int fake_ldev_task (void * arg)
{
    int             step = 0;
    int             status = 0;
    struct location location = {0, 0};

    while (1) {
        if (step & 1) {
            location_update_status (status--);
        } else {
            location_update_location (&location);
            location.x++;
            location.y++;
            location.y++;
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

static int location_user_task (void * arg)
{
    if (!location_service_start ()) {
        printf ("Fail to start service!\n");
        return -1;
    }

    location_service_listen_position (__usr_pos_callback, 0);

    location_service_listen_state (__usr_stt_callback, 0);

    printf ("listening...\n\r");

    return 0;
}

static bool_t __fake_ldev_start (void)
{
    fake_location_device_task = osal_task_create ("ldev", fake_ldev_task,
                                                  NULL, 0x800, NULL, 5);

    if (fake_location_device_task == NULL) {
        printf ("Fail to start the fake location device!\n");
    } else {
        printf ("Fake location device started!\n");
    }

    return fake_location_device_task != NULL;
}

static bool_t __fake_ldev_stop (void)
{
    if (fake_location_device_task != NULL) {
        osal_task_kill (fake_location_device_task);
        fake_location_device_task = NULL;
    }

    return true;
}

void location_test (void)
{
    location_service_config (__fake_ldev_start, __fake_ldev_stop);

    osal_task_create ("ldev", location_user_task, NULL, 0x800, NULL, 5);
}
#endif
