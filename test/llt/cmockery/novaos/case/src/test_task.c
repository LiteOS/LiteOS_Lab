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
#include "sem.h"
#include "heap.h"

/** test task interfaces */

#define TEST_CASE_WAIT_TICK     5

static volatile int dummy = 0;
static sem_id s_id;

static int task_1 (uintptr_t arg)
    {
    assert_int_equal (1, task_prio_get (current));
    assert_int_equal (0x1234, arg);
    kprintf ("task_1 run...\n");
    return 0;
    }

static int task_2 (uintptr_t arg)
    {
    kprintf ("task_2 run...\n");
    task_exit ();
    assert_true (0); // We should never be here
    return 0;
    }

static int task_3 (uintptr_t arg)
    {
    kprintf ("task_3 run...\n");
    dummy++; // 1
    task_delay (1);
    dummy++; // 2
    assert_int_equal (-1, sem_wait (s_id)); // resume task, make sem_wait failed
    dummy++; // 3
    task_delay (1);
    dummy = 0;
    return 0;
    }

static int task_4 (uintptr_t arg)
    {
    kprintf ("task_4 run...\n");
    dummy++; // 1
    task_delay (10);
    dummy++; // 2
    task_delay (10);
    dummy = 0;
    return 0;
    }

static int task_5 (uintptr_t arg)
    {
    kprintf ("task_5 run...\n");
    dummy++; // 1
    task_delay (0);
    dummy++; // 2
    task_delay (1);
    dummy++; // 3
    task_delay (3);
    dummy = 0;
    return 0;
    }

static int task_6 (uintptr_t arg)
    {
    int prio = task_prio_get (current);
    kprintf ("task_6 run...prio is %d\n", prio);
    task_delay (2);
    assert_int_not_equal (prio, task_prio_get (current));
    task_delay (2);
    assert_int_equal (prio, task_prio_get (current));
    kprintf ("task_6 end\n");
    return 0;
    }

static int task_7 (uintptr_t arg)
    {
    kprintf ("task_7 run...\n");
    dummy++; // 1
    task_delay (10);
    dummy++; // 2
    sem_wait (s_id);
    dummy++; // 3
    task_delay (2);
    dummy++; // 4
    task_delay (2);
    dummy = 0;
    return 0;
    }

void test_task_init (void **state)
    {
    task_t t;
    char *stack = heap_alloc_align (kernel_heap, STACK_ALIGN, 0x800);

    assert_int_equal (-1, task_init (NULL, NULL, NULL, 0, 0, 0, NULL, 0));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_TCB);
    assert_int_equal (-1, task_init (&t, NULL, NULL, 0, 0, 0, NULL, 0));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_STACK);
    assert_int_equal (-1, task_init (&t, stack, NULL, 0, 0, 0, NULL, 0));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_PRIO);
    assert_int_equal (-1, task_init (&t, stack + 1, "test", 0, 0, 0, NULL, 0));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_STACK);
    assert_int_equal (-1, task_init (&t, stack, "test", 0, 0, 1, NULL, 0));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_STACK);
    assert_int_equal (-1, task_init (&t, stack, "test", TASK_PRIO_MAX + 1, 0, 0, NULL, 0));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_PRIO);
    assert_int_equal (-1, task_init (&t, stack, "test", 0, 0, 0, NULL, 0)); // 0 priority for deferred task
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_PRIO);

    assert_int_equal (0, task_init (&t, stack, "test", 1, 0, 0x800, task_1, 0x1234));
    assert_int_equal (0, task_delete (&t));
    task_delay (5);

    assert_int_equal (0, task_init (&t, stack, CONFIG_DEFERRED_NAME, 0, 0, 0x800, task_1, 0));
    assert_int_equal (0, task_delete (&t));
    task_delay (5);

    heap_free (kernel_heap, stack);
    }

void test_task_create (void **state)
    {
    task_id t_id;

    assert_false (t_id = task_create ("test", 0, 0, 0, NULL, 0));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_PRIO);

    assert_true (t_id = task_create ("test", 1, 0, 0, task_1, 0x1234));
    assert_int_equal (0, task_delete (t_id));
    task_delay (5);

    assert_true (t_id = task_create ("test", 1, 0, 0, task_1, 0x1234));
    task_resume (t_id);
    task_delay (5);
    assert_int_equal (-1, task_delete (t_id));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_task_delete (void **state)
    {
    task_t t;
    task_id t_id;

    memset (&t, 0, sizeof (task_t));

    assert_int_equal (-1, task_delete (NULL));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_TCB);
    assert_int_equal (-1, task_delete (&t));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    assert_int_equal (-1, task_delete ((task_id)0x1234));
    // assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    assert_true (errno == ERRNO_OBJ_ILLEGAL_MAGIC ||
                 errno == ERRNO_TASK_ILLEGAL_OPERATION);

    assert_true (t_id = task_create ("test", 1, 0, 32, task_1, 0x1234));
    assert_int_equal (0, task_delete (t_id));
    task_delay (5); // wait task_1 over

    assert_int_equal (-1, task_delete (t_id)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_task_exit (void **state)
    {
    task_id t_id;

    assert_true (t_id = task_spawn ("test", 2, 0, 0, task_2, 0));
    task_delay (1);
    }

void test_task_resume (void **state)
    {
    task_t t;
    task_id t_id;

    assert_int_equal (-1, task_resume (NULL));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_TCB);
    assert_int_equal (-1, task_resume (&t));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    s_id = sem_create (0);
    assert_true (t_id = task_create ("test", 3, 0, 0, task_3, 0));
    task_delay (1);
    assert_int_equal (0, dummy);
    assert_int_equal (0, task_resume (t_id));
    task_delay (1);
    assert_int_equal (1, dummy);
    task_delay (1);
    assert_int_equal (2, dummy);
    task_delay (1);
    assert_int_equal (2, dummy); // task_3 wait on sem
    assert_int_equal (0, task_suspend (t_id));
    task_delay (1);
    assert_int_equal (2, dummy);
    assert_int_equal (0, task_resume (t_id)); // resume task directly, ignore the sem
                                              // so no need post sem
    task_delay (1);
    assert_int_equal (3, dummy);
    task_delay (1);

    sem_destroy (s_id);
    }

void test_task_suspend (void **state)
    {
    task_t t;
    task_id t_id;

    assert_int_equal (-1, task_suspend (NULL));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_TCB);
    assert_int_equal (-1, task_suspend (&t));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_OPERATION);

    assert_true (t_id = task_spawn ("test", 4, 0, 0, task_4, 0));
    task_delay (10);
    assert_int_equal (1, dummy);
    assert_int_equal (0, task_suspend (t_id));
    task_delay (10);
    assert_int_equal (1, dummy);
    task_resume (t_id);
    task_delay (10);
    assert_int_equal (2, dummy);
    task_delay (10);
    }

void test_task_spawn (void **state)
    {
    task_id t_id;

    assert_false (t_id = task_spawn ("test", 0, 0, 0, NULL, 0));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_PRIO);

    assert_true (t_id = task_spawn ("test", 1, 0, 0, task_1, 0x1234));
    assert_int_equal (0, task_delete (t_id));

    assert_true (t_id = task_spawn ("test", 1, 0, 0, task_1, 0x1234));
    task_delay (5);
    assert_int_equal (-1, task_delete (t_id));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_task_delay (void **state)
    {
    task_id t_id;

    task_delay (TEST_CASE_WAIT_TICK); // wait amoment, wait for the task to stabilize,
                    // otherwise cases may interact with each other

    assert_int_equal (0, task_delay (0));

    assert_true (t_id = task_spawn ("test", 1, 0, 32, task_5, 0));
    task_delay (1);
    // assert_int_equal (2, dummy); // critical
    if (2 == dummy)
        {
        assert_false (t_id->status & TASK_STATUS_DELAY);
        }
    else
        {
        assert_int_equal (3, dummy);
        }
    task_delay (1);
    // assert_int_equal (3, dummy); // It could be 4 (critical)
    assert_true (t_id->status & TASK_STATUS_DELAY);
    task_delay (2);
    }

void test_task_status_get (void **state)
    {
    task_id t_id;

    task_delay (TEST_CASE_WAIT_TICK); // wait amoment, wait for the task to stabilize,
                    // otherwise cases may interact with each other

    s_id = sem_create (0);
    assert_true (t_id = task_spawn ("test", 1, 0, 0, task_7, 0));
    task_delay (9);
    assert_int_equal (1, dummy);
    assert_false (TASK_STATUS_READY & task_status_get (t_id));
    assert_true (TASK_STATUS_DELAY & task_status_get (t_id)); // get the status, task_7 need delay 2 tick at least

    task_delay (2);
    assert_int_equal (2, dummy);
    assert_false (TASK_STATUS_DELAY & task_status_get (t_id));
    assert_true (TASK_STATUS_PEND & task_status_get (t_id));

    sem_post (s_id);
    task_delay (1);
    assert_int_equal (3, dummy);
    assert_true (TASK_STATUS_DELAY & task_status_get (t_id));
    assert_false (TASK_STATUS_PEND & task_status_get (t_id));

    task_suspend (t_id);
    task_delay (1);
    assert_int_equal (3, dummy);
    assert_true (TASK_STATUS_DELAY & task_status_get (t_id));
    assert_true (TASK_STATUS_SUSPEND & task_status_get (t_id));

    task_resume (t_id);
    task_delay (1);
    assert_int_equal (4, dummy);
    assert_true (TASK_STATUS_DELAY & task_status_get (t_id));
    assert_false (TASK_STATUS_SUSPEND & task_status_get (t_id));

    task_delay (2);
    assert_int_equal (0, dummy);
    assert_true (TASK_STATUS_INVALID & task_status_get (t_id));

    sem_destroy (s_id);
    }

void test_task_prio_get (void **state)
    {
    task_t t;
    task_id t_id;

    task_delay (TEST_CASE_WAIT_TICK); // wait amoment, wait for the task to stabilize,
                    // otherwise cases may interact with each other

    assert_int_equal (TASK_PRIO_INV, task_prio_get (&t));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_true (t_id = task_create ("test", 10, 0, 32, task_1, 0x1234));
    assert_int_equal (10, task_prio_get (t_id));

    assert_int_equal (0, task_delete (t_id)); // will resume task_1
    assert_int_equal (10, task_prio_get (t_id));
    task_delay (5); // wait task_1 over
    assert_int_equal (TASK_PRIO_INV, task_prio_get (t_id));

    assert_true (t_id = task_spawn ("test", 4, 0, 32, task_4, 0));
    assert_int_equal (4, task_prio_get (t_id));
    task_delay (10);
    assert_int_equal (0, task_delete (t_id));
    task_delay (10); // wait task_4 over
    assert_int_equal (TASK_PRIO_INV, task_prio_get (t_id));
    }

void test_task_prio_set (void **state)
    {
    task_id t_id;

    task_delay (TEST_CASE_WAIT_TICK); // wait amoment, wait for the task to stabilize,
                    // otherwise cases may interact with each other

    assert_int_equal (-1, task_prio_set (NULL, 1));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_TCB);

    assert_true (t_id = task_spawn ("test", 10, 0, 0x400, task_6, 0));
    task_delay (1);
    assert_int_equal (0, task_prio_set (t_id, 5));
    task_delay (2);
    assert_int_equal (0, task_prio_set (t_id, 10));
    task_delay (2);
    assert_int_equal (-1, task_prio_set (t_id, TASK_PRIO_MAX + 1));
    assert_int_equal (errno, ERRNO_TASK_ILLEGAL_PRIO);
    }

/** test task application scene */

static mutex_t m0;
static task_id ta, tb;

static void __sleep (volatile bool * wake)
    {
    while (!*wake)
        {
        task_delay (1);
        }

    *wake = false;
    }

static volatile bool __w_a = false;
static volatile bool __w_b = false;

#define __wake_a()      (__w_a = true)
#define __sleep_a()     __sleep(&__w_a)
#define __wake_b()      (__w_b = true)
#define __sleep_b()     __sleep(&__w_b)

static void task_a (void)
    {
    kprintf ("task A start, priority is %d\n", task_prio_get (current));

    __sleep_a ();

    // want to lock, but the owner is task_b, so task_b raise priority to task_a
    mutex_lock (&m0);

    __sleep_a ();

    mutex_unlock (&m0);

    kprintf ("task A end\n");
    }

static void task_b (void)
    {
    kprintf ("task B start, priority is %d\n", task_prio_get (current));

    mutex_lock (&m0);
    __wake_a (); 
    __sleep_b ();

    mutex_unlock (&m0);
    __sleep_b ();

    kprintf ("task B end\n");
    }

/**
 * task:    A > B
 * mutex:   m0
 *
 * app1: set prio when priority inheritance
 *       B lock, A want to lock, A will be suspended and B raise priority，
 *
 */
void test_task_app_prio (void **state)
    {
    mutex_init (&m0);

    task_delay (TEST_CASE_WAIT_TICK);

    ta = task_spawn ("a", 18, 0, 0x400, (int (*) (uintptr_t)) task_a, 0);
    tb = task_spawn ("b", 20, 0, 0x400, (int (*) (uintptr_t)) task_b, 0);

    task_delay (5);

    assert_int_equal (18, task_prio_get(tb)); // tb raise prio to ta
    assert_int_equal (0, task_prio_set (tb, 19));
    assert_int_equal (18, task_prio_get(tb)); // adjust 19 to 18

    __wake_b ();
    task_delay (5);

    assert_int_equal (19, task_prio_get(tb)); // restore to 19

    __wake_a ();
    __wake_b ();
    task_delay (5);

    mutex_destroy (&m0);
    }

/** test entry */

void test_task (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_task_init),
        unit_test (test_task_create),
        unit_test (test_task_delete),
        unit_test (test_task_exit),
        unit_test (test_task_spawn),
        unit_test (test_task_delay),
        unit_test (test_task_resume),
        unit_test (test_task_suspend),
        unit_test (test_task_status_get),
        unit_test (test_task_prio_get),
        unit_test (test_task_prio_set),

        unit_test (test_task_app_prio),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_task", test_task, NULL, NULL, 0);