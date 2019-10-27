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

#include <stdbool.h>

#include <task.h>
#include <sem.h>
#include <tick.h>
#include <cmder.h>
#include <bug.h>

task_id t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, ta, tb, tc;

static __noreturn void tick_q_test_task (uintptr_t n)
    {
    static unsigned int adjs [3] = {1, (unsigned int) -2, 1};
    int                 i = 0;

    while (1)
        {
        unsigned int expected = n + tick_count_get ();

        task_delay ((unsigned int) n);

        if (expected != tick_count_get ())
            {
            n = tick_count_get ();
            __bug ("system tick count not expected!");
            expected += n;
            }

        n += adjs [i++ % 3];
        }
    }

static int __cmd_tick_test (cmder_t * cmder, int argc, char * argv [])
    {
    t0 = task_spawn ("t0", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 10);
    t1 = task_spawn ("t1", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 6);
    t2 = task_spawn ("t2", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 8);
    t3 = task_spawn ("t3", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 1);
    t4 = task_spawn ("t4", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 10);
    t5 = task_spawn ("t5", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 8);
    t6 = task_spawn ("t6", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 1);
    t7 = task_spawn ("t7", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 6);
    t8 = task_spawn ("t8", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 11);
    t9 = task_spawn ("t9", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 15);
    ta = task_spawn ("ta", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 20);
    tb = task_spawn ("tb", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 20);
    tc = task_spawn ("tc", 0, 0, 0x500, (int (*) (uintptr_t)) tick_q_test_task, 20);

    return 0;
    }

CMDER_CMD_DEF ("tick_test", "tick test", __cmd_tick_test);

