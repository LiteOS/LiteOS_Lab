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
#include "timer.h"
#include "task.h"

/** test timer interfaces */

static volatile int once = 0;
static volatile int perd = 0;

static void __timer_once (uintptr_t arg)
    {
    assert_int_equal (0x1234, arg);
    kprintf ("%d in __timer_once\n", ++once);
    }

static void __timer_perd (uintptr_t arg)
    {
    assert_int_equal (0x4321, arg);
    kprintf ("%d in __timer_perd\n", ++perd);
    }

void test_timer_init (void **state)
    {
    timer_t t;
    memset (&t, 0, sizeof (timer_t));

    assert_int_equal (-1, timer_init (NULL, 0, 3, NULL, 0));
    assert_int_equal (errno, ERRNO_TIMER_ILLEGAL_ID);
    assert_int_equal (-1, timer_init (&t, 0, 3, NULL, 0));
    assert_int_equal (errno, ERRNO_TIMER_ILLEGAL_PFN);
    assert_int_equal (-1, timer_init (&t, TIMER_FLAG_REPEATED + 1, 3, NULL, 0));
    assert_int_equal (errno, ERRNO_TIMER_ILLEGAL_FLAG);
    assert_int_equal (-1, timer_init (&t, 0, 0, __timer_once, 0));
    assert_int_equal (errno, ERRNO_TIMER_ILLEGAL_INTERVAL);

    assert_int_equal (0, timer_init (&t, 0, 3, __timer_once, 0));
    assert_int_equal (-1, timer_init (&t, 1, 3, __timer_perd, 0)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_OPERATION);
    assert_int_equal (0, timer_delete (&t));
    }

void test_timer_delete (void **state)
    {
    timer_t t;
    memset (&t, 0, sizeof (timer_t));

    assert_int_equal (-1, timer_delete (NULL));
    assert_int_equal (errno, ERRNO_TIMER_ILLEGAL_ID);
    assert_int_equal (-1, timer_delete (&t));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (0, timer_init (&t, 0, 3, __timer_once, 0));
    assert_int_equal (0, timer_delete (&t));
    assert_int_equal (-1, timer_delete (&t)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (-1, timer_delete ((timer_id)0x1234)); // invalid address
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_timer_create (void **state)
    {
    timer_id t1_id;
    timer_id t2_id;
    timer_id t3_id;

    assert_false (timer_create (0, 3, NULL, 0));
    assert_int_equal (errno, ERRNO_TIMER_ILLEGAL_PFN);
    assert_false (timer_create (TIMER_FLAG_REPEATED + 1, 3, NULL, 0));
    assert_int_equal (errno, ERRNO_TIMER_ILLEGAL_FLAG);

    assert_true (t1_id = timer_create (0, 3, __timer_once, 0));
    assert_true (t2_id = timer_create (0, 3, __timer_once, 0));
    assert_true (t3_id = timer_create (0, 3, __timer_once, 0));

    // do not destroy in order
    assert_int_equal (0, timer_delete (t3_id));
    assert_int_equal (0, timer_delete (t1_id));
    assert_int_equal (0, timer_delete (t2_id));

    assert_int_equal (-1, timer_delete (t1_id)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_timer_start (void **state)
    {
    timer_t t;
    timer_id t1_id = NULL;
    timer_id t2_id = NULL;

    assert_int_equal (-1, timer_start (NULL));
    assert_int_equal (errno, ERRNO_TIMER_ILLEGAL_ID);
    assert_int_equal (-1, timer_start (&t));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    assert_int_equal (-1, timer_start ((timer_id)0x1234));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_true (t1_id = timer_create (0, 3, __timer_once, 0x1234));
    assert_true (t2_id = timer_create (1, 4, __timer_perd, 0x4321));

    assert_int_equal (0, timer_start (t1_id));
    assert_int_equal (0, timer_start (t2_id));

    task_delay (10);
    assert_int_equal (1, once);
    assert_int_equal (2, perd);
    once = perd = 0;

    // delete
    assert_int_equal (0, timer_delete (t1_id));
    assert_int_equal (0, timer_delete (t2_id));
    }

void test_timer_stop (void **state)
    {
    timer_t t;
    timer_id t1_id = NULL;
    timer_id t2_id = NULL;

    assert_int_equal (-1, timer_stop (NULL));
    assert_int_equal (errno, ERRNO_TIMER_ILLEGAL_ID);
    assert_int_equal (-1, timer_stop (&t));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    assert_int_equal (-1, timer_stop ((timer_id)0x1234));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_true (t1_id = timer_create (0, 30, __timer_once, 0x1234));
    assert_int_equal (0, timer_start (t1_id));

    task_delay (20);
    assert_int_equal (0, timer_stop (t1_id));
    task_delay (10);
    assert_int_equal (0, once);
    assert_int_equal (0, timer_start (t1_id)); // restart, recount
    task_delay (20);
    assert_int_equal (0, once);
    task_delay (10);
    assert_int_equal (1, once);
    assert_int_equal (0, timer_delete (t1_id)); // delete

    assert_true (t2_id = timer_create (1, 40, __timer_perd, 0x4321));
    assert_int_equal (0, timer_start (t2_id));

    task_delay (40); // critical
    assert_int_equal (1, perd);
    assert_int_equal (0, timer_stop (t2_id));
    assert_int_equal (0, timer_start (t2_id)); // restart, recount
    task_delay (60);
    assert_int_equal (2, perd);
    assert_int_equal (0, timer_delete (t2_id)); // delete
    task_delay (30);
    assert_int_equal (2, perd); // still 2

    once = perd = 0;
    }

/** test entry */

void test_timer (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_timer_init),
        unit_test (test_timer_delete),
        unit_test (test_timer_create),
        unit_test (test_timer_start),
        unit_test (test_timer_stop),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_timer", test_timer, NULL, NULL, 0);