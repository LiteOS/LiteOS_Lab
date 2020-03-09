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

#ifdef CONFIG_TASK_CREATE_HOOK
extern int            task_create_hook_del (task_create_pfn);
#endif
#ifdef CONFIG_TASK_DELETE_HOOK
extern int            task_delete_hook_del (task_delete_pfn);
#endif
#ifdef CONFIG_TASK_SWITCH_HOOK
extern int            task_switch_hook_del (task_switch_pfn);
#endif

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

/** test task hook interfaces */

// task_delete_hook_add, task_switch_hook_add like this
void test_task_create_hook_add (void **state)
    {
    int i = 0;

    for (i = 0; i < CONFIG_NR_TASK_CREATE_HOOK_SLOTS; ++i)
        {
        assert_int_equal (0, task_create_hook_add (__create_hook));
        }
    assert_int_equal (-1, task_create_hook_add (__create_hook));

    for (i = CONFIG_NR_TASK_CREATE_HOOK_SLOTS / 2; i > 0; --i)
        {
        assert_int_equal (0, task_create_hook_del (__create_hook));
        }
    for (i = CONFIG_NR_TASK_CREATE_HOOK_SLOTS / 2 + 1; i < CONFIG_NR_TASK_CREATE_HOOK_SLOTS; ++i)
        {
        assert_int_equal (0, task_create_hook_del (__create_hook));
        }
    if (CONFIG_NR_TASK_CREATE_HOOK_SLOTS > 0)
        {
        assert_int_equal (0, task_create_hook_del (__create_hook));
        }
    }

static void __test_hook_init (void)
    {
    task_create_hook_add (__create_hook);
    task_delete_hook_add (__delete_hook);
    task_switch_hook_add (__switch_hook);
    }

static void __test_hook_exit (void)
    {
    task_create_hook_del (__create_hook);
    task_delete_hook_del (__delete_hook);
    task_switch_hook_del (__switch_hook);
    }

static void task_1 (void)
    {
    task_delay (2);
    }

static void task_2 (void)
    {
    task_delay (3);
    }

void test_task_hook_demo (void **state)
    {
    __test_hook_init ();

    task_spawn ("task_1", 18, 0, 0x400, (int (*) (uintptr_t)) task_1, 0);
    task_spawn ("task_2", 19, 0, 0x400, (int (*) (uintptr_t)) task_2, 0);

    task_delay (5);

    __test_hook_exit ();
    }

/** test entry */

void test_hook (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_task_create_hook_add),
        unit_test (test_task_hook_demo),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_hook", test_hook, NULL, NULL, 0);