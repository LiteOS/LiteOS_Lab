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

extern void context_test (uintptr_t);

static __noreturn int tls_test (uintptr_t magic)
    {
    task_tls_set (current, 0, magic);

    while (1)
        {
        if (magic != 0)
            {
            uintptr_t t;

            task_tls_get (current, 0, &t);

            kprintf ("expected %08x, got %08x\n", magic, t);
            }

        task_delay (CONFIG_SYS_TICK_HZ);
        }
    }

static int __cmd_tls_test (cmder_t * cmder, int argc, char * argv [])
    {
    task_spawn ("tls0", 30, 0, 0x400, tls_test, 0x11111111);
    task_spawn ("tls1", 30, 0, 0x400, tls_test, 0x22222222);
    task_spawn ("tls2", 20, 0, 0x400, tls_test, 0x33333333);

    return 0;
    }

CMDER_CMD_DEF ("tls_test", "tls test", __cmd_tls_test);
