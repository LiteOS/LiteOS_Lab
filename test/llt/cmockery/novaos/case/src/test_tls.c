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

#include "tester.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "cmockery.h"

#include "kprintf.h"
#include "task.h"

static volatile int task_counter = 0;

static int task_tls (uintptr_t magic)
    {
    int i = 0;
    task_tls_set (current, 0, magic);

    while (i++ < 3)
        {
        if (magic != 0)
            {
            uintptr_t t;

            task_tls_get (current, 0, &t);

            kprintf ("expected %08x, got %08x\n", magic, t);
            }

        task_delay (1);
        }
    task_counter++;
    return 0;
    }

void test_task_tls_demo (void **state)
    {
    task_spawn ("tls0", 30, 0, 0x400, task_tls, 0x11111111);
    task_spawn ("tls1", 30, 0, 0x400, task_tls, 0x22222222);
    task_spawn ("tls2", 20, 0, 0x400, task_tls, 0x33333333);

    while (task_counter < 3)
        {
        task_delay (10);
        }
    }

/** test entry */

void test_tls (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_task_tls_demo),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_tls", test_tls, NULL, NULL, 0);