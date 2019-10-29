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

extern void context_test (uintptr_t);

static void loop (uintptr_t magic)
    {
    context_test (magic);
    }

static __noreturn void delay (void)
    {
    while (1)
        {
        task_delay (100);
        }
    }

static int __cmd_task_test (cmder_t * cmder, int argc, char * argv [])
    {
    task_spawn ("loop0", 30, 0, 0x100, (int (*) (uintptr_t)) loop,  0xdead);
    task_spawn ("loop1", 30, 0, 0x100, (int (*) (uintptr_t)) loop,  0xbeef);
    task_spawn ("delay", 20, 0, 0x100, (int (*) (uintptr_t)) delay, 0);

    return 0;
    }

CMDER_CMD_DEF ("task_test", "task test", __cmd_task_test);
