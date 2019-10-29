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

#include <task.h>
#include <sem.h>
#include <mutex.h>

#include <cmder.h>
#include <bug.h>

mutex_t m0, m1, m2, m3;

task_id t18, t19, t20, t21, t22, t23, t24, t25, t26, t27;

volatile int dummy = 0;

void p27 (void)
    {
    mutex_lock (&m0);

    while (task_prio_get (current) != 26)
        {
        dummy++;
        }

    while (task_prio_get (current) != 20)
        {
        dummy++;
        }

    task_delay (1);     // let p20 to go, as we are 19!

    // p18 will executive first in next tick, and we will be 18 after that

    if ((task_prio_get (current) != 18) || (task_prio_get (t24) != 18))
        {
        dummy++;
        }

    mutex_unlock (&m0);

    if (task_prio_get (current) != 27)
        {
        __bug ("current prio not expected, should be 27!");
        }
    }

void p26 (void)
    {
    task_delay (1);

    mutex_lock (&m0);

    mutex_unlock (&m0);

    if (task_prio_get (current) != 26)
        {
        __bug ("current prio not expected, should be 26!");
        }
    }

void p25 (void)
    {
    task_delay (4);

    mutex_lock (&m1);

    mutex_unlock (&m1);

    if (task_prio_get (current) != 25)
        {
        __bug ("current prio not expected, should be 25!");
        }
    }

void p24 (void)
    {
    task_delay (1);

    mutex_lock (&m1);
    mutex_lock (&m2);

    task_delay (1);

    if (task_prio_get (current) != 23)
        {
        __bug ("current prio not expected, should be 23!");
        }

    task_delay (1);

    if (task_prio_get (current) != 22)
        {
        __bug ("current prio not expected, should be 22!");
        }

    task_delay (2);

    if (task_prio_get (current) != 21)
        {
        __bug ("current prio not expected, should be 21!");
        }

    task_delay (1);

    if (task_prio_get (current) != 19)
        {
        __bug ("current prio not expected, should be 19!");
        }

    if (task_prio_get (t22) != 19)
        {
        __bug ("t22 prio not expected, should be 19!");
        }

    task_delay (1);

    /*
     * t19 timedout, so t24 back to 21, t22 back to 22, but t20 just try to
     * took m2, so t24->20
     */

    if (task_prio_get (current) != 20)
        {
        __bug ("current prio not expected, should be 20!");
        }

    if (task_prio_get (t22) != 22)
        {
        __bug ("t22 prio not expected, should be 22!");
        }

    mutex_lock (&m0);

    mutex_unlock (&m1);

    if (task_prio_get (current) != 18)
        {
        __bug ("current prio not expected, should be 18!");
        }

    mutex_unlock (&m2);

    if (task_prio_get (current) != 24)
        {
        __bug ("current prio not expected, should be 24!");
        }

    mutex_unlock (&m0);
    }

void p23 (void)
    {
    task_delay (2);

    mutex_lock (&m1);
    mutex_unlock (&m1);

    if (task_prio_get (current) != 23)
        {
        __bug ("current prio not expected, should be 23!");
        }
    }

void p22 (void)
    {
    task_delay (3);

    mutex_lock (&m3);
    mutex_lock (&m1);
    mutex_unlock (&m1);
    mutex_unlock (&m3);
    }

void p21 (void)
    {
    task_delay (5);
    mutex_lock (&m1);
    mutex_unlock (&m1);
    }

void p20 (void)
    {
    task_delay (7);

    if (task_prio_get (t24) != 21)
        {
        __bug ("current prio not expected, should be 21!");
        }

    if (task_prio_get (t22) != 22)
        {
        __bug ("current prio not expected, should be 22!");
        }

    mutex_lock (&m2);
    }

void p19 (void)
    {
    task_delay (6);
    mutex_timedlock (&m3, 1);
    }

void p18 (void)
    {
    task_delay (8);
    mutex_lock (&m2);
    mutex_unlock (&m2);
    }

static int __cmd_mutex_test (cmder_t * cmder, int argc, char * argv [])
    {
    mutex_init (&m0);
    mutex_init (&m1);
    mutex_init (&m2);
    mutex_init (&m3);

    t27 = task_spawn ("p27", 27, 0, 0x400, (int (*) (uintptr_t)) p27, 0);
    t26 = task_spawn ("p26", 26, 0, 0x400, (int (*) (uintptr_t)) p26, 0);
    t25 = task_spawn ("p25", 25, 0, 0x400, (int (*) (uintptr_t)) p25, 0);
    t24 = task_spawn ("p24", 24, 0, 0x400, (int (*) (uintptr_t)) p24, 0);
    t23 = task_spawn ("p23", 23, 0, 0x400, (int (*) (uintptr_t)) p23, 0);
    t22 = task_spawn ("p22", 22, 0, 0x400, (int (*) (uintptr_t)) p22, 0);
    t21 = task_spawn ("p21", 21, 0, 0x400, (int (*) (uintptr_t)) p21, 0);
    t20 = task_spawn ("p20", 20, 0, 0x400, (int (*) (uintptr_t)) p20, 0);
    t19 = task_spawn ("p19", 19, 0, 0x400, (int (*) (uintptr_t)) p19, 0);
    t18 = task_spawn ("p18", 18, 0, 0x400, (int (*) (uintptr_t)) p18, 0);

    return 0;
    }

CMDER_CMD_DEF ("mutex_test", "mutex test", __cmd_mutex_test);

