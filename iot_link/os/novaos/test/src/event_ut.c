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

#include <config.h>
#include <event.h>
#include <task.h>
#include <cmder.h>
#include <kprintf.h>
#include <bug.h>

static event_t event;
static event_t done;

static int done_cnt = 0;

void e1 (void)
    {
    uint32_t r;

    kprintf ("task e1 recv 0x11, ALL\n");
    event_recv (&event, 0x11, EVENT_WAIT_ALL | EVENT_WAIT_CLR, &r);
    kprintf ("task e4 wake up\n");

    if (done_cnt != 3)
        {
        __bug ("done count should be 3!");
        }

    event_send (&done, 0x1);

    done_cnt++;
    }

void e2 (void)
    {
    uint32_t r;

    kprintf ("task e2 recv 0x22, ANY\n");
    event_recv (&event, 0x22, EVENT_WAIT_ANY | EVENT_WAIT_CLR, &r);
    kprintf ("task e2 wake up\n");

    if (done_cnt != 0)
        {
        __bug ("done count should be 0!");
        }

    event_send (&done, 0x2);

    done_cnt++;
    }

void e3 (void)
    {
    uint32_t r;

    kprintf ("task e3 recv 0x41, ALL\n");
    event_recv (&event, 0x41, EVENT_WAIT_ALL | EVENT_WAIT_CLR, &r);
    kprintf ("task e3 wake up\n");

    if (done_cnt != 2)
        {
        __bug ("done count should be 2!");
        }

    event_send (&done, 0x4);

    done_cnt++;
    }

void e4 (void)
    {
    uint32_t r;

    kprintf ("task e4 recv 0x22, ALL\n");
    event_recv (&event, 0x22, EVENT_WAIT_ALL | EVENT_WAIT_CLR, &r);
    kprintf ("task e4 wake up\n");

    if (done_cnt != 1)
        {
        __bug ("done count should be 1!");
        }

    event_send (&done, 0x8);

    done_cnt++;
    }

void e5 (void)
    {
    task_delay (CONFIG_SYS_TICK_HZ);
    kprintf ("e5 sending 0x2\n");
    event_send (&event, 0x2);
    kprintf ("e5 sending 0x2\n");
    event_send (&event, 0x2);
    kprintf ("e5 sending 0x21\n");
    event_send (&event, 0x21);
    kprintf ("e5 sending 0x40\n");
    event_send (&event, 0x40);
    kprintf ("e5 sending 0x10\n");
    event_send (&event, 0x10);
    kprintf ("e5 sending 0x1\n");
    event_send (&event, 0x1);
    }

void e6 (void)
    {
    uint32_t r;
    int      ret;

    kprintf ("task e6 recv 0x8f, ALL, 5 second (will timeout)\n");

    ret = event_timedrecv (&event, 0x8f, EVENT_WAIT_ALL | EVENT_WAIT_CLR, 5 * CONFIG_SYS_TICK_HZ, &r);

    if (ret != -1)
        {
        __bug ("timeout test fail!");
        }

    if (errno != ERRNO_EVENT_TIMEOUT)
        {
        __bug ("timeout test fail!");
        }

    kprintf ("task e6 wake up because timed out\n");

    event_send (&done, 0x10);

    done_cnt++;
    }

static int __cmd_event_test (cmder_t * cmder, int argc, char * argv [])
    {
    event_init (&event);
    event_init (&done);

    done_cnt = 0;

    task_spawn ("e6", 5, 0, 0x400, (int (*) (uintptr_t)) e6, 0);
    task_spawn ("e5", 5, 0, 0x400, (int (*) (uintptr_t)) e5, 0);
    task_spawn ("e4", 4, 0, 0x400, (int (*) (uintptr_t)) e4, 0);
    task_spawn ("e3", 3, 0, 0x400, (int (*) (uintptr_t)) e3, 0);
    task_spawn ("e2", 2, 0, 0x400, (int (*) (uintptr_t)) e2, 0);
    task_spawn ("e1", 1, 0, 0x400, (int (*) (uintptr_t)) e1, 0);

    event_recv (&done, 0x1f, EVENT_WAIT_ALL | EVENT_WAIT_CLR, NULL);

    kprintf ("test pass!\n");

    return 0;
    }

CMDER_CMD_DEF ("event_test", "event test", __cmd_event_test);

