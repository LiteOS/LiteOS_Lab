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

#include <msg_queue.h>
#include <task.h>
#include <sem.h>
#include <kprintf.h>
#include <cmder.h>
#include <bug.h>

mq_id mq;

task_id tm1, tm2, tm3;
static  sem_t sem;

void mq1 (void)
    {
    char buff [8] = {1, 2, 3, 4, 5, 6, 7, 8};
    kprintf ("task mq1 sending 12345678\n");
    mq_send (mq, buff, 8);
    }

void mq2 (void)
    {
    int i;

    char buff [8] = {0};
    mq_recv (mq, buff, 8);

    kprintf ("task mq2 recved\n");

    for (i = 0; i < 8; i++)
        {
        if (buff [i] != i + 1)
            {
            __bug ("message content verify wrong!");
            }
        }

    kprintf ("task mq2 verified content\n");

    mq_timedrecv (mq, buff, 8, 5);

    kprintf ("task mq2 recved\n");

    for (i = 0; i < 8; i++)
        {
        if (buff [i] != 8 - i)
            {
            __bug ("message content verify wrong!");
            }
        }

    kprintf ("task mq2 verified content\n");

    sem_post (&sem);
    }

void mq3 (void)
    {
    char buff [8] = {8, 7, 6, 5, 4, 3, 2, 1};
    kprintf ("task mq3 sending 87654321\n");
    mq_send (mq, buff, 8);
    }

static int __cmd_mq_test (cmder_t * cmder, int argc, char * argv [])
    {
    mq = mq_create (8, 64, 0);

    sem_init (&sem, 0);

    tm1 = task_spawn ("m1", 0, 0, 0x400, (int (*) (uintptr_t)) mq1, 0);
    tm2 = task_spawn ("m2", 1, 0, 0x400, (int (*) (uintptr_t)) mq2, 0);
    tm3 = task_spawn ("m3", 2, 0, 0x400, (int (*) (uintptr_t)) mq3, 0);

    sem_wait (&sem);

    kprintf ("test pass!\n");

    return 0;
    }

CMDER_CMD_DEF ("mq_test", "message queue test", __cmd_mq_test);
