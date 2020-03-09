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
#include "defer.h"

static deferred_job_t job1, job2;

static void __job_1 (uintptr_t arg)
    {
    kprintf ("__job_1...\n");
    assert_int_equal (0x1234, arg);
    }

static void __job_2 (uintptr_t arg)
    {
    kprintf ("__job_2...\n");
    assert_int_equal (0x4321, arg);
    }

void test_defer_demo (void **state)
    {
    deferred_job_init (&job1, __job_1, 0x1234);
    deferred_job_init (&job2, __job_2, 0x4321);

    task_delay (1);

    deferred_job_sched (&job1);

    task_delay (1);

    deferred_job_sched (&job2);

    task_delay (1);
    }

/** test entry */

void test_defer (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_defer_demo),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_defer", test_defer, NULL, NULL, 0);