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
#include "mutex.h"
#include "sem.h"
#include "task.h"

/** test mutex interfaces */

void test_mutex_init (void **state)
    {
    mutex_t m;
    memset (&m, 0, sizeof (mutex_t));

    assert_int_equal (-1, mutex_init (NULL));
    assert_int_equal (errno, ERRNO_MUTEX_ILLEGAL_ID);

    assert_int_equal (0, mutex_init (&m));
    assert_int_equal (-1, mutex_init (&m)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_OPERATION);
    assert_int_equal (0, mutex_destroy (&m));
    }

void test_mutex_destory (void **state)
    {
    mutex_t m;
    memset (&m, 0, sizeof (mutex_t));

    assert_int_equal (-1, mutex_destroy (NULL));
    assert_int_equal (errno, ERRNO_MUTEX_ILLEGAL_ID);

    assert_int_equal (-1, mutex_destroy (&m));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (0, mutex_init (&m));
    assert_int_equal (0, mutex_destroy (&m));
    assert_int_equal (-1, mutex_destroy (&m)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (-1, mutex_destroy ((mutex_id)0x1234)); // invalid address
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_mutex_create (void **state)
    {
    mutex_id m1_id;
    mutex_id m2_id;
    mutex_id m3_id;

    assert_true (m1_id = mutex_create ());
    assert_true (m2_id = mutex_create ());
    assert_true (m3_id = mutex_create ());

    // do not destroy in order
    assert_int_equal (0, mutex_destroy (m3_id));
    assert_int_equal (0, mutex_destroy (m1_id));
    assert_int_equal (0, mutex_destroy (m2_id));

    assert_int_equal (-1, mutex_destroy (m1_id)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_mutex_open (void **state)
    {
    mutex_id m1_id = NULL;
    mutex_id m2_id = NULL;

    assert_false (m1_id = mutex_open (NULL, 0));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_OPERATION);

    // If the file does not exist, it is created, and if it exists, it is returned
    assert_true (m1_id = mutex_open ("m1", O_CREAT));
    assert_true (m2_id = mutex_open ("m1", O_CREAT));
    assert_int_equal (m1_id, m2_id);

    assert_int_equal (0, mutex_destroy (m2_id)); // decrese open_cnt (take care the open-count)

    // If O_CREAT is also specified and the file already exists, an error is returned.
    assert_false (m2_id = mutex_open ("m1", O_CREAT | O_EXCL));
    assert_int_equal (errno, EEXIST);

    assert_false (m2_id = mutex_open ("m2", 0));
    assert_int_equal (errno, ENOENT);
    assert_false (m2_id = mutex_open ("m2", O_EXCL));
    assert_int_equal (errno, ENOENT);

    // destroy
    assert_int_equal (0, mutex_destroy (m1_id));
    }

// mutex_trylock, mutex_timedlock like this
void test_mutex_lock (void **state)
    {
    mutex_t m;
    memset (&m, 0, sizeof (mutex_t));

    assert_int_equal (-1, mutex_lock (NULL));
    assert_int_equal (errno, ERRNO_MUTEX_ILLEGAL_ID);
    assert_int_equal (-1, mutex_lock (&m)); // should be initialized firstly
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (0, mutex_init (&m));

    assert_int_equal (0, mutex_lock (&m));
    assert_int_equal (0, mutex_unlock (&m));
    assert_int_equal (-1, mutex_unlock (&m)); // many times to unlock
    assert_int_equal (errno, ERRNO_MUTEX_ILLEGAL_OPERATION);

    assert_int_equal (0, mutex_lock (&m));
    assert_int_equal (0, mutex_lock (&m));

    assert_int_equal (0, mutex_unlock (&m));
    assert_int_equal (0, mutex_destroy (&m)); // early destroy (take care)
    assert_int_equal (-1, mutex_unlock (&m));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    // reinitialization is required after destruction
    assert_int_equal (-1, mutex_lock (&m));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_mutex_unlock (void **state)
    {
    mutex_t m;
    memset (&m, 0, sizeof (mutex_t));

    assert_int_equal (-1, mutex_unlock (NULL));
    assert_int_equal (errno, ERRNO_MUTEX_ILLEGAL_ID);
    assert_int_equal (-1, mutex_unlock (&m)); // should be initialized firstly
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (0, mutex_init (&m));
    assert_int_equal (-1, mutex_unlock (&m)); // should be locked firstly
    assert_int_equal (errno, ERRNO_MUTEX_ILLEGAL_OPERATION);

    assert_int_equal (0, mutex_destroy (&m));
    }

/** test mutex application scene */

static volatile int dummy = 0;
static mutex_t m0;
static task_id ta, tb, tc, td;

static sem_t sem;
static void init (void)
    {
    sem_init (&sem, 0);
    }

static void quit (void)
    {
    sem_post (&sem);
    }

static void wait (int task_num)
    {
    int i;
    for (i = 0; i < task_num; i++)
        {
        sem_wait (&sem);
        }
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

    // 1)
    mutex_lock (&m0);
    dummy++;
    __sleep_a (); // schedule task_b, that will be suspended

    // 3)
    mutex_unlock (&m0);
    __sleep_a (); // wake up task_b

    // 5)
    assert_int_equal (2, dummy);
    __wake_b ();
    // want to lock, but the owner is task_b, so task_b raise priority to task_a
    mutex_lock (&m0); // wait here

    // 7)
    assert_int_equal (1, dummy);
    dummy++; // 2
    kprintf ("task A lock and sleep ~~\n");
    __wake_d ();   // lock sleep, task D B C will wait
    __sleep_a ();

    // 11)
    mutex_unlock (&m0); // will wake up task_b

    kprintf ("task A end\n");
    quit ();
    }

static void task_b (void)
    {
    kprintf ("task B start, priority is %d\n", task_prio_get (current));

    // 2)
    __wake_a ();
    mutex_lock (&m0); // wait here, because of task_a lock

    // 4)
    assert_int_equal (1, dummy);
    dummy++;
    __wake_a ();
    __sleep_b (); // shcedule task_a

    // 6)
    // now task_b priority is equal to task_a, check it
    assert_int_equal (task_prio_get (ta), task_prio_get (tb));
    kprintf ("task B c_prio: %d, o_prio: %d\n", tb->c_prio, tb->o_prio);

    dummy--;
    mutex_unlock (&m0); // wake up task_a
    // restore task_b priority
    kprintf ("task B c_prio: %d, o_prio: %d\n", tb->c_prio, tb->o_prio);

    __sleep_b (); // sleep

    // 9)
    kprintf ("task B request lock, will be suspended\n");
    __wake_c ();
    mutex_lock (&m0); // wait here

    // 12)
    kprintf ("task B c_prio: %d, o_prio: %d\n", tb->c_prio, tb->o_prio);
    assert_int_equal (2, dummy);
    dummy++;
    mutex_unlock (&m0);

    kprintf ("task B end\n");
    quit ();
    }

static void task_c (void)
    {
    kprintf ("task C start, priority is %d\n", task_prio_get (current));

    __sleep_c ();

    // 10)
    kprintf ("task C request lock, will be suspended\n");
    __wake_a ();
    mutex_lock (&m0); // wait here

    // 13)
    kprintf ("task C c_prio: %d, o_prio: %d\n", tc->c_prio, tc->o_prio);
    assert_int_equal (3, dummy);
    dummy++;
    mutex_unlock (&m0);

    kprintf ("task C end\n");
    quit ();
    }

static void task_d (void)
    {
    kprintf ("task D start, priority is %d\n", task_prio_get (current));

    __sleep_d ();

    // 8)
    kprintf ("task D request lock, will be suspended\n");
    __wake_b ();
    mutex_lock (&m0); // wait here

    // 14)
    kprintf ("task D c_prio: %d, o_prio: %d\n", td->c_prio, td->o_prio);
    assert_int_equal (4, dummy);
    mutex_unlock (&m0);

    kprintf ("task D end\n");
    quit ();
    }

/**
 * task:    A > B > C > D
 * mutex:   m0
 *
 * app1: typical mutex, resource contention, after A to B                   (-step: 1,2,3,4
 *          B, A            B    |-> A
 *             |-> m0       |-> m0
 *
 * app2: priority inheritance
 *       B lock, A want to lock, A will be suspended and B raise priority   (-step: 4,5,6,7
 *             B            A
 *             |-> m0   <---|
 *
 * app3: priority flip problem                              (-step: 7,8,9,10,11,12,13,14
 *       A lock, D B C request lock in turn, the waiting order will be B C D (by task priority)
 *
 */
void test_mutex_app_simple (void **state)
    {
    dummy = 0;
    mutex_init (&m0);

    init ();

    ta = task_spawn ("a", 18, 0, 0x400, (int (*) (uintptr_t)) task_a, 0);
    tb = task_spawn ("b", 19, 0, 0x400, (int (*) (uintptr_t)) task_b, 0);
    tc = task_spawn ("c", 20, 0, 0x400, (int (*) (uintptr_t)) task_c, 0);
    td = task_spawn ("d", 21, 0, 0x400, (int (*) (uintptr_t)) task_d, 0);

    wait (4);

    mutex_destroy (&m0);
    }

/** test entry */

void test_mutex (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_mutex_init),
        unit_test (test_mutex_destory),
        unit_test (test_mutex_create),
        unit_test (test_mutex_open),
        unit_test (test_mutex_lock),
        unit_test (test_mutex_unlock),

        unit_test (test_mutex_app_simple),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_mutex", test_mutex, NULL, NULL, 0);