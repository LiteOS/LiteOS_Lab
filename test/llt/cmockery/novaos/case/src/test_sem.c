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
#include "sem.h"
#include "task.h"

/** test sem interfaces */

void test_sem_init (void **state)
    {
    sem_t s;
    memset (&s, 0, sizeof (sem_t));

    assert_int_equal (-1, sem_init (NULL, 0));
    assert_int_equal (errno, ERRNO_SEM_ILLEGAL_ID);

    assert_int_equal (0, sem_init (&s, 1));
    assert_int_equal (-1, sem_init (&s, 2)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_OPERATION);
    assert_int_equal (0, sem_destroy (&s));
    }

void test_sem_destory (void **state)
    {
    sem_t s;
    memset (&s, 0, sizeof (sem_t));

    assert_int_equal (-1, sem_destroy (NULL));
    assert_int_equal (errno, ERRNO_SEM_ILLEGAL_ID);
    assert_int_equal (-1, sem_destroy (&s));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);

    assert_int_equal (0, sem_init (&s, 1));
    assert_int_equal (0, sem_destroy (&s));
    assert_int_equal (-1, sem_destroy (&s)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);

    assert_int_equal (-1, sem_destroy ((sem_id)0x1234)); // invalid address
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    }

void test_sem_create (void **state)
    {
    sem_id s1_id;
    sem_id s2_id;
    sem_id s3_id;

    assert_true (s1_id = sem_create (1));
    assert_true (s2_id = sem_create (1));
    assert_true (s3_id = sem_create (1));

    // do not destroy in order
    assert_int_equal (0, sem_destroy (s3_id));
    assert_int_equal (0, sem_destroy (s1_id));
    assert_int_equal (0, sem_destroy (s2_id));

    assert_int_equal (-1, sem_destroy (s1_id)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    }

void test_sem_open (void **state)
    {
    sem_id s1_id = NULL;
    sem_id s2_id = NULL;

    assert_false (s1_id = sem_open (NULL, 0));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_OPERATION);

    // If the file does not exist, it is created, and if it exists, it is returned
    assert_true (s1_id = sem_open ("s1", O_CREAT));
    assert_true (s2_id = sem_open ("s1", O_CREAT));
    assert_int_equal (s1_id, s2_id);

    assert_int_equal (0, sem_destroy (s2_id)); // decrese open_cnt (take care the open-count)

    // If O_CREAT is also specified and the file already exists, an error is returned.
    assert_false (s2_id = sem_open ("s1", O_CREAT | O_EXCL));
    assert_int_equal (errno, EEXIST);

    assert_false (s2_id = sem_open ("s2", 0));
    assert_int_equal (errno, ENOENT);
    assert_false (s2_id = sem_open ("s2", O_EXCL));
    assert_int_equal (errno, ENOENT);

    // destroy
    assert_int_equal (0, sem_destroy (s1_id));
    }

// sem_trywait, sem_timedwait like this
void test_sem_wait (void **state)
    {
    sem_t s;

    assert_int_equal (-1, sem_wait (NULL));
    assert_int_equal (errno, ERRNO_SEM_ILLEGAL_ID);
    assert_int_equal (-1, sem_wait (&s)); // should be initialized firstly
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (0, sem_init (&s, 1));

    // init value is 1
    assert_int_equal (0, sem_wait (&s));
    assert_int_equal (-1, sem_trywait (&s));
    assert_int_equal (errno, ERRNO_SEM_UNAVAILABLE);
    assert_int_equal (-1, sem_timedwait (&s, 2));
    assert_int_equal (errno, ERRNO_SEM_TIMEOUT);
    // assert_int_equal (-1, sem_wait (&s)); // self wait

    assert_int_equal (0, sem_post (&s));
    assert_int_equal (0, sem_post (&s)); // many times to post is ok

    // now value is 2
    assert_int_equal (2, sem_getvalue (&s));
    assert_int_equal (0, sem_wait (&s));
    assert_int_equal (0, sem_wait (&s));

    assert_int_equal (0, sem_post (&s));
    assert_int_equal (0, sem_destroy (&s)); // early destroy (take care)
    assert_int_equal (-1, sem_post (&s));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    // reinitialization is required after destruction
    assert_int_equal (-1, sem_wait (&s));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_sem_post (void **state)
    {
    sem_t s;

    assert_int_equal (-1, sem_post (NULL));
    assert_int_equal (errno, ERRNO_SEM_ILLEGAL_ID);
    assert_int_equal (-1, sem_post (&s)); // should be initialized firstly
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (0, sem_init (&s, 1));

    assert_int_equal (0, sem_post (&s));    // max value is UINT_MAX
    assert_int_equal (2, sem_getvalue (&s));

    assert_int_equal (0, sem_destroy (&s));

    // test critical condition
    assert_int_equal (0, sem_init (&s, UINT_MAX));

    assert_int_equal (-1, sem_post (&s));
    assert_int_equal (errno, ERRNO_SEM_OVERFLOW);
    assert_int_equal (UINT_MAX, sem_getvalue (&s));

    assert_int_equal (0, sem_destroy (&s));
    }

void test_sem_getvalue (void **state)
    {
    sem_t s;
    memset (&s, 0, sizeof (sem_t));

    assert_int_equal (0, sem_getvalue (&s));

    assert_int_equal (0, sem_init (&s, 1));
    assert_int_equal (1, sem_getvalue (&s));
    assert_int_equal (0, sem_destroy (&s));
    }

/** test sem application scene */

static volatile int dummy = 0;
static int quit_flag = 0;
static sem_t s0;
static task_id ta, tb, tc, td;

static void quit (void)
    {
    quit_flag = 1;
    }

static void wait (void)
    {
    while (!quit_flag)
        {
        task_delay (10);
        }
    quit_flag = 0;
    }

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
static volatile bool __w_c = false;
static volatile bool __w_d = false;

#define __wake_a()      (__w_a = true)
#define __sleep_a()     __sleep(&__w_a)
#define __wake_b()      (__w_b = true)
#define __sleep_b()     __sleep(&__w_b)
#define __wake_c()      (__w_c = true)
#define __sleep_c()     __sleep(&__w_c)
#define __wake_d()      (__w_d = true)
#define __sleep_d()     __sleep(&__w_d)

static void task_a (void)
    {
    kprintf ("task A start, priority is %d\n", task_prio_get (current));

    /**
     * binary sem, resource contention
     */
    // 1)
    sem_wait (&s0);
    __sleep_a (); // schedule task_b, that will be suspended

    // 3)
    dummy++; // 1
    sem_post (&s0);
    __sleep_a (); // wake up task_b

    // 5)
    assert_int_equal (2, dummy);
    __wake_b ();
    sem_wait (&s0); // suspended here, because of task_B hold sem

    // 7)
    assert_int_equal (1, dummy);
    /**
     *  thread synchronization
     */
    __wake_b (); // shcedule task_b
    __sleep_a ();

    // 9)
    dummy++; // 2
    sem_post (&s0);
    __sleep_a (); // waka up task_b to receive data

    // 11)
    /**
     * priority flip problem
     */
    sem_wait (&s0);
    dummy++; // 3
    kprintf ("task A hold sem and sleep ~~\n");
    __wake_d ();
    __sleep_a ();   // hold sleep, task D B C will wait

    // 15)
    sem_post (&s0); // will wake up task_d

    kprintf ("task A end\n");
    }

static void task_b (void)
    {
    kprintf ("task B start, priority is %d\n", task_prio_get (current));

    /**
     * binary sem, resource contention
     */
    // 2)
    __wake_a ();
    sem_wait (&s0); // suspended here, because of task_a hold sem

    // 4)
    assert_int_equal (1, dummy);
    dummy++; // 2
    __wake_a ();
    __sleep_b (); // shcedule task_a

    // 6)
    // check b priority, not raising
    kprintf ("task B c_prio: %d, o_prio: %d\n", tb->c_prio, tb->o_prio);
    dummy--; // 1
    sem_post (&s0);
    __sleep_b (); // wake up task_a

    /**
     *  thread synchronization
     */
    // 8)
    __wake_a ();
    sem_wait (&s0); // suspended here, wait task_a data

    // 10)
    assert_int_equal (2, dummy);
    sem_post (&s0);

    /**
     * priority flip problem
     */
    __wake_a ();
    __sleep_b (); // sleep

    // 13)
    kprintf ("task B request sem, will be suspended\n");
    __wake_c ();
    sem_wait (&s0); // suspended here

    // 17)
    kprintf ("task B c_prio: %d, o_prio: %d\n", tb->c_prio, tb->o_prio);
    assert_int_equal (4, dummy);
    dummy++; // 5
    sem_post (&s0); // B > C, so run continue

    kprintf ("task B end\n");
    }

static void task_c (void)
    {
    kprintf ("task C start, priority is %d\n", task_prio_get (current));

    __sleep_c ();

    /**
     * priority flip problem
     */
    // 14)
    kprintf ("task C request sem, will be suspended\n");
    __wake_a ();
    sem_wait (&s0); // suspended here

    // 18)
    kprintf ("task C c_prio: %d, o_prio: %d\n", tc->c_prio, tc->o_prio);
    assert_int_equal (5, dummy);
    dummy++;
    sem_post (&s0);

    kprintf ("task C end\n");
    }

static void task_d (void)
    {
    kprintf ("task D start, priority is %d\n", task_prio_get (current));

    __sleep_d ();

    /**
     * priority flip problem
     */
    // 12)
    kprintf ("task D request sem, will be suspended\n");
    __wake_b ();
    sem_wait (&s0); // suspended here

    // 16)
    kprintf ("task D c_prio: %d, o_prio: %d\n", td->c_prio, td->o_prio);
    assert_int_equal (3, dummy);
    dummy++; // 4
    sem_post (&s0); // B > D, so wake up task_b

    kprintf ("task D end\n");
    quit ();
    }

/**
 * task:  A > B > C > D
 * sem:   s0
 *
 * app1: binary sem, resource contention        (-step: 1,2,2,4,5,6
 *       1) A prior to B
 *       2) B prior to A
 *
 * app2: thread synchronization                 (-step: 7,8,9,10
 *       1) B wait to receive data from A
 *
 * app3: priority flip problem
 *       1) A run out of semaphore resources,   (-step: 11,12,13,14,15,16,17,18
 *          D B C request sem in turn, the waiting order will be D B C (by request order)
 *
 * note:
 *       1) binary sem not support priority inheritance, you can use mutex
 *
 */
void test_sem_app_simple (void **state)
    {
    dummy = 0;
    sem_init (&s0, 1);

    ta = task_spawn ("a", 18, 0, 0x400, (int (*) (uintptr_t)) task_a, 0);
    tb = task_spawn ("b", 19, 0, 0x400, (int (*) (uintptr_t)) task_b, 0);
    tc = task_spawn ("c", 20, 0, 0x400, (int (*) (uintptr_t)) task_c, 0);
    td = task_spawn ("d", 21, 0, 0x400, (int (*) (uintptr_t)) task_d, 0);

    wait ();

    sem_destroy (&s0);
    }

/** test entry */

void test_sem (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_sem_init),
        unit_test (test_sem_destory),
        unit_test (test_sem_create),
        unit_test (test_sem_open),
        unit_test (test_sem_wait),
        unit_test (test_sem_post),
        unit_test (test_sem_getvalue),

        unit_test (test_sem_app_simple),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_sem", test_sem, NULL, NULL, 0);