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

#include <timer.h>
#include <task.h>
#include <bug.h>
#include <kprintf.h>
#include <cmder.h>

timer_t tim1, tim2;

static void __timer_once (uintptr_t arg)
    {
    if (arg != 0x1234)
        __bug ("timer arguments not expected (0x1234)!");
    }

static void __timer_perd (uintptr_t arg)
    {
    if (arg != 0x4321)
        __bug ("timer arguments not expected (0x4321)!");

    if (tick_count_get () & 1)
        __bug ("system tick count not expected (odd values)!");

    kprintf (".");
    }

static int __cmd_timer_test (cmder_t * cmder, int argc, char * argv [])
    {
    timer_init (&tim1, 0, 3, __timer_once, 0x1234);
    timer_init (&tim2, 1, CONFIG_SYS_TICK_HZ, __timer_perd, 0x4321);

    if (tick_count_get () & 1)
        {
        task_delay (1);
        }

    timer_start (&tim1);
    timer_start (&tim2);

    return 0;
    }

CMDER_CMD_DEF ("timer_test", "timer test", __cmd_timer_test);

