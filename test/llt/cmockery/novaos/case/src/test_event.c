/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 *     http://license.coscl.org.cn/MulanPSL
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
#include "event.h"
#include "task.h"

/** test event interfaces */

void test_event_init (void **state)
    {
    event_t e;
    memset (&e, 0, sizeof (event_t));

    assert_int_equal (-1, event_init (NULL));
    assert_int_equal (errno, ERRNO_EVENT_ILLEGAL_ID);

    assert_int_equal (0, event_init (&e));
    assert_int_equal (-1, event_init (&e)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_OPERATION);
    assert_int_equal (0, event_destroy (&e));
    }

void test_event_destory (void **state)
    {
    event_t e;
    memset (&e, 0, sizeof (event_t));

    assert_int_equal (-1, event_destroy (NULL));
    assert_int_equal (errno, ERRNO_EVENT_ILLEGAL_ID);
    assert_int_equal (-1, event_destroy (&e));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);

    assert_int_equal (0, event_init (&e));
    assert_int_equal (0, event_destroy (&e));
    assert_int_equal (-1, event_destroy (&e)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);

    assert_int_equal (-1, event_destroy ((event_id)0x1234)); // invalid address
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    }

void test_event_create (void **state)
    {
    event_id e1_id;
    event_id e2_id;
    event_id e3_id;

    assert_true (e1_id = event_create ());
    assert_true (e2_id = event_create ());
    assert_true (e3_id = event_create ());

    // do not destroy in order
    assert_int_equal (0, event_destroy (e3_id));
    assert_int_equal (0, event_destroy (e1_id));
    assert_int_equal (0, event_destroy (e2_id));

    assert_int_equal (-1, event_destroy (e1_id)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    }

void test_event_open (void **state)
    {
    event_id e1_id = NULL;
    event_id e2_id = NULL;

    assert_false (e1_id = event_open (NULL, 0));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_OPERATION);

    // If the file does not exist, it is created, and if it exists, it is returned
    assert_true (e1_id = event_open ("e1", O_CREAT));
    assert_true (e2_id = event_open ("e1", O_CREAT));
    assert_int_equal (e1_id, e2_id);

    assert_int_equal (0, event_destroy (e2_id)); // decrese open_cnt (take care the open-count)

    // If O_CREAT is also specified and the file already exists, an error is returned.
    assert_false (e2_id = event_open ("e1", O_CREAT | O_EXCL));
    assert_int_equal (errno, EEXIST);

    assert_false (e2_id = event_open ("e2", 0));
    assert_int_equal (errno, ENOENT);
    assert_false (e2_id = event_open ("e2", O_EXCL));
    assert_int_equal (errno, ENOENT);

    // destroy
    assert_int_equal (0, event_destroy (e1_id));
    }

// event_tryrecv, event_timedrecv like this
void test_event_recv (void **state)
    {
    event_t e;
    uint32_t wanted = 0x03;
    uint32_t option = EVENT_WAIT_ALL | EVENT_WAIT_CLR;
    uint32_t recved;

    assert_int_equal (-1, event_recv (NULL, 0, 0, &recved));
    assert_int_equal (errno, ERRNO_EVENT_ILLEGAL_ID);
    assert_int_equal (-1, event_recv (&e, 0, 0, &recved));      // option invalid
    assert_int_equal (errno, ERRNO_EVENT_ILLEGAL_OPERATION);
    assert_int_equal (-1, event_recv (&e, 0, option, &recved)); // e uninitialized
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (0, event_init (&e));

    assert_int_equal (0, event_recv (&e, 0, option, &recved)); // e.event_set = 0;
    assert_int_equal (-1, event_tryrecv (&e, wanted, option, &recved));
    assert_int_equal (errno, ERRNO_EVENT_UNAVAILABLE);
    assert_int_equal (-1, event_timedrecv (&e, wanted, option, 2, &recved));
    assert_int_equal (errno, ERRNO_EVENT_TIMEOUT);

    assert_int_equal (0, event_send (&e, 0x01));
    assert_int_equal (0x01, e.event_set);
    assert_int_equal (-1, event_tryrecv (&e, wanted, EVENT_WAIT_ALL, &recved));
    assert_int_equal (errno, ERRNO_EVENT_UNAVAILABLE);
    assert_int_equal (0, event_tryrecv (&e, wanted, EVENT_WAIT_ANY, &recved));
    assert_int_equal (0x01, recved);
    assert_int_equal (0, event_tryrecv (&e, wanted, EVENT_WAIT_ANY | EVENT_WAIT_CLR, &recved));
    assert_int_equal (0x01, recved);
    assert_int_equal (0, e.event_set);

    assert_int_equal (0, event_send (&e, wanted));
    assert_int_equal (0, event_recv (&e, wanted, EVENT_WAIT_ALL, &recved));
    assert_int_equal (wanted, recved);
    assert_int_equal (0, event_destroy (&e)); // early destroy (take care)
    assert_int_equal (-1, event_tryrecv (&e, wanted, EVENT_WAIT_ALL | EVENT_WAIT_CLR, &recved));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_event_send (void **state)
    {
    event_t e;

    assert_int_equal (-1, event_send (NULL, 0));
    assert_int_equal (errno, ERRNO_EVENT_ILLEGAL_ID);
    assert_int_equal (-1, event_send (&e, 0));      // events invalid
    assert_int_equal (errno, ERRNO_EVENT_ILLEGAL_OPERATION);
    assert_int_equal (-1, event_send (&e, 0x01));   // e uninitialized
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (0, event_init (&e));

    assert_int_equal (0, event_send (&e, 0x01));
    assert_int_equal (0x01, e.event_set);
    assert_int_equal (0, event_send (&e, 0x01));
    assert_int_equal (0x01, e.event_set);
    assert_int_equal (0, event_send (&e, 0x02));
    assert_int_equal (0x03, e.event_set);

    assert_int_equal (0, event_destroy (&e));
    }

/** test event application scene */

static volatile int dummy = 0;
static int quit_flag = 0;
static event_t e0;
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
    uint32_t r;
    kprintf ("task A start, priority is %d\n", task_prio_get (current));

    /**
     * app1: thread synchronization (basic)
     */
    // 1)
    // e0 (0) dummy (0)
    dummy++;
    event_send (&e0, 0x01);

    __sleep_a ();

    // 3)
    // e0 (1) dummy (0)
    assert_int_equal (0, dummy);
    dummy++;

    __wake_b (); __sleep_a ();

    // 5)
    // e0 (1) dummy (0)
    assert_int_equal (0, dummy);
    dummy++;
    event_send (&e0, 0x01);

    __wake_b (); __sleep_a ();

    // 7)
    // e0 (1) dummy (0)
    assert_int_equal (0, dummy);
    dummy++;
    event_send (&e0, 0x02);

    __wake_b (); __sleep_a ();

    // 10)
    // e0 (3) dummy (1)
    assert_int_equal (1, dummy);
    dummy++;
    event_send (&e0, 0x03);

    __wake_b (); __sleep_a ();

    // 13)
    // e0 (2) dummy (1)
    assert_int_equal (1, dummy);
    dummy++;
    event_send (&e0, 0x01);
    // e0 (3) dummy (2)

    __wake_b (); __sleep_a ();

    // 16)
    // e0 (0) dummy (3)
    assert_int_equal (3, dummy);
    dummy++;
    event_send (&e0, 0x02);
    // e0 (2) dummy (4)
    // fall thru

    /**
     * app2: thread synchronization (more to one, more to more)
     */
    // e0 (2) dummy (4)
    // next event_recv (3) will suspend, 17) in task_b will run
    event_recv (&e0, 0x03, EVENT_WAIT_ALL | EVENT_WAIT_CLR, &r);
    // waked up by 18) in task_b and 19) in task_c
    // e0 (0) dummy (4)
    // fall thru to 20)

    // 20)
    // e0 (0) dummy (4)
    assert_int_equal (4, dummy);
    dummy++;

    __wake_b ();
    event_recv (&e0, 0x02, EVENT_WAIT_ALL, &r);
    // waked up by 23) in task_d
    // e0 (3) dummy (6)
    // fall thru to 24)

    // 24)
    // e0 (3) dummy (6)
    assert_int_equal (6, dummy);
    dummy++;

    __sleep_a ();

    // 26)
    // e0 (0) dummy (6)
    assert_int_equal (6, dummy);
    dummy++;
    /**
     * app3: priority flip problem
     */
    // 27)
    // e0 (0) dummy (7)
    kprintf ("task A sleep ~~\n");
    __wake_d ();
    __sleep_a ();

    // 31)
    // e0 (0) dummy (7)
    kprintf ("task A send event 0x03\n");
    event_send (&e0, 0x03);

    kprintf ("task A end\n");
    }

static void task_b (void)
    {
    uint32_t r;
    kprintf ("task B start, priority is %d\n", task_prio_get (current));

    /**
     * app1: thread synchronization (basic)
     */
    // 2)
    // e0 (1) dummy (1)
    event_recv (&e0, 0x01, EVENT_WAIT_ANY, &r);
    assert_int_equal (1, dummy);
    dummy--;

    __wake_a (); __sleep_b ();

    // 4)
    // e0 (1) dummy (1)
    event_recv (&e0, 0x01, EVENT_WAIT_ANY, &r);
    assert_int_equal (1, dummy);
    dummy--;

    __wake_a (); __sleep_b ();

    // 6)
    // e0 (1) dummy (1)
    event_recv (&e0, 0x01, EVENT_WAIT_ANY, &r);
    assert_int_equal (1, dummy);
    dummy--;

    __wake_a (); __sleep_b ();

    // 8)
    // e0 (3) dummy (1)
    event_recv (&e0, 0x01, EVENT_WAIT_ANY, &r);
    assert_int_equal (1, dummy);
    dummy--;

    // now task_c can be resumed by the event, just sleep let it run
    __sleep_b ();

    // 11)
    // e0 (3) dummy (2)
    event_recv (&e0, 0x01, EVENT_WAIT_ANY | EVENT_WAIT_CLR, &r);
    // e0 (2) dummy (2)
    assert_int_equal (2, dummy);
    dummy--;

    __wake_c (); __sleep_b ();

    // 14)
    // e0 (3) dummy (2)
    event_recv (&e0, 0x02, EVENT_WAIT_ALL | EVENT_WAIT_CLR, &r); // now task_c can be resumed by the event

    // waked up by 16) in task_a
    // e0 (0) dummy (4)
    // fall thru to 17)

    // 17)
    // e0 (0) dummy (4)
    assert_int_equal (4, dummy);
    dummy--;

    /**
     * app2: thread synchronization (more to one, more to more)
     */
    // 18)
    // e0 (0) dummy (3)
    event_send (&e0, 0x01);

    __wake_c (); __sleep_b ();

    // 21)
    // e0 (0) dummy (5)
    // schedule as the recv in 20) task_a cause task_a suspended
    assert_int_equal (5, dummy);
    dummy--;

    __wake_c ();
    event_recv (&e0, 0x03, EVENT_WAIT_ALL | EVENT_WAIT_CLR, &r);// suspend as e0 is 0
    // wake up by 22) in task_c and 23 in task_d
    // run after 24) in task_a
    // e0 (0) dummy (7)
    // fall thru to 25)

    // 25)
    // e0 (0) dummy (7)
    assert_int_equal (7, dummy);
    dummy--;

    __wake_a (); __sleep_b ();

    /**
     * app3: priority flip problem
     */
    // 29)
    // e0 (0) dummy (7)
    kprintf ("task B want event 0x01 (ANY), will be suspended\n");
    __wake_c ();
    event_recv (&e0, 0x01, EVENT_WAIT_ANY, &r); // suspended here

    // 32)
    // e0 (3) dummy (7)
    kprintf ("task B c_prio: %d, o_prio: %d\n", tb->c_prio, tb->o_prio);
    assert_int_equal (7, dummy);
    dummy++;

    kprintf ("task B end\n");
    }

static void task_c (void)
    {
    uint32_t r;
    kprintf ("task C start, priority is %d\n", task_prio_get (current));

    /**
     * app1: thread synchronization (basic)
     */
    // 9)
    // e0 (3) dummy (0)
    event_recv (&e0, 0x03, EVENT_WAIT_ALL, &r);
    assert_int_equal (0, dummy);
    dummy++;

    __wake_a (); __sleep_c ();

    // 12)
    // e0 (2) dummy (1)
    __wake_a ();
    event_recv (&e0, 0x03, EVENT_WAIT_ALL | EVENT_WAIT_CLR, &r);

    // waked up by 14) in task_b
    // e0 (0) dummy (2)
    // fall thru to 15)

    // 15)
    // e0 (0) dummy (2)
    assert_int_equal (2, dummy);
    dummy++;

    __wake_a (); __sleep_c ();

    /**
     * app2: thread synchronization (more to one, more to more)
     */
    // 19)
    // e0 (1) dummy (3)
    assert_int_equal (3, dummy);
    dummy++;
    event_send (&e0, 0x02);
    // e0 (3) dummy (4)

    // now task_c can be resumed by the event, just sleep let it run in 20)
    __sleep_c ();

    // 22)
    // e0 (0) dummy (4)
    assert_int_equal (4, dummy);
    dummy++;
    event_send (&e0, 0x01);

    __wake_d (); __sleep_c ();

    /**
     * app3: priority flip problem
     */
    // 30)
    // e0 (0) dummy (7)
    kprintf ("task C want event 0x02 (ANY), will be suspended\n");
    __wake_a ();
    event_recv (&e0, 0x02, EVENT_WAIT_ANY, &r); // suspended here

    // 33)
    // e0 (3) dummy (8)
    kprintf ("task C c_prio: %d, o_prio: %d\n", tc->c_prio, tc->o_prio);
    assert_int_equal (8, dummy);
    dummy++;

    kprintf ("task C end\n");
    }

static void task_d (void)
    {
    uint32_t r;
    kprintf ("task D start, priority is %d\n", task_prio_get (current));

    __sleep_d ();

    /**
     * app2: thread synchronization (more to one, more to more)
     */
    // 23)
    // e0 (1) dummy (5)
    assert_int_equal (5, dummy);
    dummy++;
    event_send (&e0, 0x02); // schedule task_a and task_b, task_a run first

    __sleep_d ();

    /**
     * app3: priority flip problem
     */
    // 28)
    // e0 (0) dummy (7)
    kprintf ("task D want event 0x03 (ANY), will be suspended\n");
    __wake_b ();
    event_recv (&e0, 0x03, EVENT_WAIT_ANY, &r); // suspended here

    // 34)
    // e0 (3) dummy (9)
    kprintf ("task D c_prio: %d, o_prio: %d\n", td->c_prio, td->o_prio);
    assert_int_equal (9, dummy);
    dummy++;

    kprintf ("task D end\n");
    quit ();
    }

/**
 * task:  A > B > C > D
 * event: e0
 *
 * app1: thread synchronization (basic)
 *       a) A send 0x01, B recv 0x01 (ANY                       (-step: 1,2
 *       b) A ..       , B reve 0x01 (ANY                       (-step: 3,4
 *       c) A send 0x01, B recv 0x01 (ANY                       (-step: 5,6
 *       d) A send 0x02, B recv 0x01 (ANY                       (-step: 7,8,9
 *                       C recv 0x03 (ALL
 *       e) A send 0x03, B recv 0x01 (ANY + CLR                 (-step: 10,11,12
 *                       C recv 0x03 (ALL           -> wait
 *       f) A send 0x01, B recv 0x02 (ALL + CLR     -> wait     (-step: 13,14,15
 *                       C recv 0x03 (ALL + CLR
 *       g) A send 0x02, B recv 0x02 (ALL + CLR                 (-step: 16,17
 *
 * app2: thread synchronization (more to one, more to more)
 *       a) A want 0x03 (ALL + CLR), B send 0x01,             C send 0x02                   (-step: 18,19,20
 *       b) A want 0x02 (ALL      ), B want 0x03 (ALL + CLR), C send 0x01, D send 0x02      (-step: 21,22,23,24,25
 *
 * app3: priority flip problem
 *       a) A is the event sender,                              (-step: 26,27,28,29,30,31,32,33,34
 *          D B C request event in turn, the waiting order will be B C D (by priority order)
 *
 */
void test_event_app_simple (void **state)
    {
    dummy = 0;
    event_init (&e0);

    ta = task_spawn ("a", 18, 0, 0x400, (int (*) (uintptr_t)) task_a, 0);
    tb = task_spawn ("b", 19, 0, 0x400, (int (*) (uintptr_t)) task_b, 0);
    tc = task_spawn ("c", 20, 0, 0x400, (int (*) (uintptr_t)) task_c, 0);
    td = task_spawn ("d", 21, 0, 0x400, (int (*) (uintptr_t)) task_d, 0);

    wait ();

    event_destroy (&e0);
    }

/** test entry */

void test_event (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_event_init),
        unit_test (test_event_destory),
        unit_test (test_event_create),
        unit_test (test_event_open),
        unit_test (test_event_recv),
        unit_test (test_event_send),

        unit_test (test_event_app_simple),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_event", test_event, NULL, NULL, 0);