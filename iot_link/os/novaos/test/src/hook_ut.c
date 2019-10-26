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
#include <cmder.h>
#include <kprintf.h>

static int __create_hook (task_id task)
    {
    kprintf ("created %s\n", task->name);
    return 0;
    }

static void __delete_hook (task_id task)
    {
    kprintf ("deleting %s\n", task->name);
    }

static void __switch_hook (task_id old, task_id new)
    {
    kprintf ("%s -> %s\n", new->name, old->name);
    }

void hook_ut_init (void)
    {
    task_create_hook_add (__create_hook);
    task_delete_hook_add (__delete_hook);
    task_switch_hook_add (__switch_hook);
    }

static void delay (void)
    {
    task_delay (100);
    task_delay (100);
    task_delay (100);
    }

static __noreturn void loop (void)
    {
    while (1) {}
    }

static int __cmd_hook_test (cmder_t * cmder, int argc, char * argv [])
    {
    task_spawn ("delay1", 20, 0, 0x400, (int (*) (uintptr_t)) delay, 0);
    task_spawn ("delay2", 20, 0, 0x400, (int (*) (uintptr_t)) delay, 0);
    task_spawn ("loop1",  30, 0, 0x400, (int (*) (uintptr_t)) loop,  0);
    task_spawn ("loop2",  30, 0, 0x400, (int (*) (uintptr_t)) loop,  0);

    return 0;
    }

CMDER_CMD_DEF ("hook_test", "tls test", __cmd_hook_test);
