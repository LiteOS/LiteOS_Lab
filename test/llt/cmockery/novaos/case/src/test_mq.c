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
#include "msg_queue.h"
#include "task.h"

/** test mq interfaces */

void test_mq_create (void **state)
    {
    mq_id q1_id;
    mq_id q2_id;
    mq_id q3_id;
    size_t max_msgs = (size_t)INT_MAX + 1U;

    assert_false (mq_create (0, 0, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_SIZE);
    assert_false (mq_create (10, 0, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_MSGS);
    assert_false (mq_create (2, max_msgs, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_MSGS);

    assert_true (q1_id = mq_create (10, 1, 0));
    assert_int_equal (16, q1_id->msg_size); // round up 8
    assert_true (q2_id = mq_create (1, 2, 0));
    assert_int_equal (8, q2_id->msg_size); // round up 8
    assert_true (q3_id = mq_create (7, 3, 0));
    assert_int_equal (8, q3_id->msg_size); // round up 8

    // do not destroy in order
    assert_int_equal (0, mq_delete (q3_id));
    assert_int_equal (0, mq_delete (q1_id));
    assert_int_equal (0, mq_delete (q2_id));

    assert_int_equal (-1, mq_delete (q1_id)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    }

void test_mq_delete (void **state)
    {
    mq_t q;
    mq_id q_id = NULL;

    memset (&q, 0, sizeof (mq_t));

    assert_int_equal (-1, mq_delete (NULL));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_ID);
    assert_int_equal (-1, mq_delete (&q));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);

    assert_true (q_id = mq_create (8, 1, 0));
    assert_int_equal (0, mq_delete (q_id));

    assert_int_equal (-1, mq_delete ((mq_id)0x1234)); // invalid address
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    }

void test_mq_open (void **state)
    {
    mq_id q1_id = NULL;
    mq_id q2_id = NULL;

    // mq_open (name, flag, mode, msg_size, max_msgs, options)
    assert_false (q1_id = mq_open (NULL, 0));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_OPERATION);
    assert_false (q1_id = mq_open ("q1", O_CREAT, 0, 0, 0, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_SIZE);
    // assert_false (q1_id = mq_open ("q1", O_CREAT, 0, 10)); // other params are random number
    assert_false (q1_id = mq_open ("q1", O_CREAT, 0, 10, 0, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_MSGS);

    // If the file does not exist, it is created, and if it exists, it is returned
    assert_true (q1_id = mq_open ("q1", O_CREAT, 0, 7, 1, 0));
    assert_true (q2_id = mq_open ("q1", O_CREAT, 0, 12, 1, 0));
    assert_int_equal (q1_id, q2_id);
    assert_int_equal (8, q1_id->msg_size);
    assert_int_equal (q1_id->msg_size, q2_id->msg_size);

    assert_int_equal (0, mq_delete (q2_id)); // decrese open_cnt (take care the open-count)

    // If O_CREAT is also specified and the file already exists, an error is returned.
    assert_false (q2_id = mq_open ("q1", O_CREAT | O_EXCL));
    assert_int_equal (errno, EEXIST);

    assert_false (q2_id = mq_open ("q2", 0));
    assert_int_equal (errno, ENOENT);
    assert_false (q2_id = mq_open ("q2", O_EXCL));
    assert_int_equal (errno, ENOENT);

    // delete
    assert_int_equal (0, mq_delete (q1_id));
    }

// mq_timedsend, mq_trysend like this
void test_mq_send (void **state)
    {
    mq_t q;
    mq_id q_id;
    char buf [] = "hello mq";
    size_t len = strlen (buf);

    assert_int_equal (-1, mq_send (NULL, NULL, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_ID);
    assert_int_equal (-1, mq_send (&q, NULL, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_BUFF);
    assert_int_equal (-1, mq_send (&q, buf, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_SIZE);
    assert_int_equal (-1, mq_send (&q, buf, len)); // q invalid
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_true (q_id = mq_create (len - 1, 2, 0));
    assert_int_equal (0, mq_send (q_id, buf, len)); // send_size = min(msg_size, len)
    assert_int_equal (0, mq_send (q_id, buf, len));
    assert_int_equal (-1, mq_trysend (q_id, buf, len)); // suspended on sem (mq is full)
    assert_int_equal (errno, ERRNO_MQ_UNAVAILABLE);
    assert_int_equal (-1, mq_timedsend (q_id, buf, len, 2));
    assert_int_equal (errno, ERRNO_MQ_TIMEOUT);

    assert_int_equal (0, mq_delete (q_id));
    }

// mq_timedrecv, mq_tryrecv like this
void test_mq_recv (void **state)
    {
    mq_t q;
    mq_id q_id;
    char buf [20] = {0};
    size_t len = sizeof (buf);

    assert_int_equal (-1, mq_recv (NULL, NULL, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_ID);
    assert_int_equal (-1, mq_recv (&q, NULL, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_BUFF);
    assert_int_equal (-1, mq_recv (&q, buf, 0));
    assert_int_equal (errno, ERRNO_MQ_ILLEGAL_SIZE);
    assert_int_equal (-1, mq_recv (&q, buf, len)); // q invalid
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    // 1)
    assert_true (q_id = mq_create (len, 2, 0));

    assert_int_equal (0, mq_send (q_id, "hello", strlen ("hello"))); // FIFO
    assert_int_equal (0, mq_send (q_id, "world", strlen ("world")));
    assert_int_equal (0, mq_recv (q_id, buf, len));
    // assert_string_equal ("hello", buf); // copy min(len, msg_size) data to buf
    assert_memory_equal ("hello", buf, strlen ("hello"));
    memset (buf, 0, len);
    assert_int_equal (0, mq_recv (q_id, buf, len));
    assert_memory_equal ("world", buf, strlen ("world"));
    memset (buf, 0, len);

    assert_int_equal (0, mq_delete (q_id));

    // 2)
    typedef struct msg
        {
        char *data;
        size_t len;
        } msg_t;

    msg_t m;
    memset (&m, 0, sizeof (msg_t));

    assert_true (q_id = mq_create (sizeof (msg_t), 2, 0));

    m.data = "hello";
    m.len = strlen (m.data) + 1;
    assert_int_equal (0, mq_send (q_id, &m, sizeof (msg_t)));
    m.data = "world";
    m.len = strlen (m.data) + 1;
    assert_int_equal (0, mq_send (q_id, &m, sizeof (msg_t)));
    assert_int_equal (0, mq_recv (q_id, &m, sizeof (msg_t)));
    assert_string_equal ("hello", m.data);
    assert_int_equal (m.len, sizeof ("hello"));
    assert_int_equal (0, mq_recv (q_id, &m, sizeof (msg_t)));
    assert_string_equal ("world", m.data);
    assert_int_equal (m.len, sizeof ("world"));

    assert_int_equal (0, mq_delete (q_id));
    }

// /** test mq application scene */

static volatile int dummy = 0;
static int quit_flag = 0;
static mq_id q0;
static task_id ta, tb, tc, td;

typedef struct msg_cb
    {
    char *packet;
    size_t len;
    } msg_cb_t;

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

static void task_a (void)
    {
    kprintf ("task A start, priority is %d\n", task_prio_get (current));

    /**
     * thread message communication
     */
    char packet [5][20] =
        {
        "1 task A",
        "2 task A (-)",
        "3 task A",
        "4 task A (*)",
        "5 task A"
        };
    int i;
    msg_cb_t cb;

    for (i = 0; i < 5; ++i)
        {
        cb.packet = packet[i];
        cb.len = strlen (cb.packet) + 1;
        mq_send (q0, &cb, sizeof (msg_cb_t));
        kprintf ("task A send data to   q0, data: %s\n", cb.packet);
        task_delay (1);
        }
    dummy++;

    task_delay (2);
    /**
     * asynchronous processing
     */
    // 1）
    for (i = 0; i < 5; ++i)
        {
        packet [i][0] = 'a' + i;
        }
    kprintf ("\n");
    for (i = 0; i < 5; ++i)
        {
        cb.packet = packet[i];
        cb.len = strlen (cb.packet) + 1;
        mq_send (q0, &cb, sizeof (msg_cb_t));
        kprintf ("task A send data to   q0, data: %s\n", cb.packet);
        task_delay (1);
        }
    dummy++;

    // 2)
    kprintf ("\n");
    for (i = 0; i < 5; ++i)
        {
        cb.packet = packet[i];
        cb.len = strlen (cb.packet) + 1;
        mq_send (q0, &cb, sizeof (msg_cb_t));
        kprintf ("task A send data to   q0, data: %s\n", cb.packet);
        task_delay (1);
        }
    dummy++;

    kprintf ("task A end\n");
    }

static void task_b (void)
    {
    kprintf ("task B start, priority is %d\n", task_prio_get (current));

    /**
     * thread message communication
     */
    msg_cb_t cb;
    while (dummy == 0)
        {
        // mq_timedrecv (q0, &cb, sizeof (msg_cb_t), 2)
        if (mq_tryrecv (q0, &cb, sizeof (msg_cb_t)) == 0)
            {
            kprintf ("task B recv data from q0, data: %s\n", cb.packet);
            }
        else
            {
            task_delay (2);
            }
        }

    task_delay (2);

    /**
     * asynchronous processing
     */
    // 1)
    while (dummy == 1)
        {
        // mq_timedrecv (q0, &cb, sizeof (msg_cb_t), 2)
        if (mq_tryrecv (q0, &cb, sizeof (msg_cb_t)) == 0)
            {
            kprintf ("task B recv data from q0, data: %s\n", cb.packet);
            }
        else
            {
            task_delay (1);
            }
        task_delay (1);
        }

    // 2)
    char packet [5][20] =
        {
        "1 task B",
        "2 task B (-)",
        "3 task B",
        "4 task B (*)",
        "5 task B"
        };
    int i;
    for (i = 0; i < 5; ++i)
        {
        cb.packet = packet[i];
        cb.len = strlen (cb.packet) + 1;
        mq_send (q0, &cb, sizeof (msg_cb_t));
        kprintf ("task B send data to   q0, data: %s\n", cb.packet);
        task_delay (1);
        }
    dummy++;

    kprintf ("task B end\n");
    }

static void task_c (void)
    {
    kprintf ("task C start, priority is %d\n", task_prio_get (current));

    task_delay (8);
    /**
     * asynchronous processing
     */
    // 1)
    msg_cb_t cb;
    while (dummy == 1)
        {
        // mq_timedrecv (q0, &cb, sizeof (msg_cb_t), 2)
        if (mq_tryrecv (q0, &cb, sizeof (msg_cb_t)) == 0)
            {
            kprintf ("task C recv data from q0, data: %s\n", cb.packet);
            }
        else
            {
            task_delay (1);
            }
        }

    // 2)
    char packet [5][20] =
        {
        "1 task C",
        "2 task C (-)",
        "3 task C",
        "4 task C (*)",
        "5 task C"
        };
    int i;
    for (i = 0; i < 5; ++i)
        {
        cb.packet = packet[i];
        cb.len = strlen (cb.packet) + 1;
        mq_send (q0, &cb, sizeof (msg_cb_t));
        kprintf ("task C send data to   q0, data: %s\n", cb.packet);
        task_delay (1);
        }
    dummy++;

    kprintf ("task C end\n");
    }

static void task_d (void)
    {
    kprintf ("task D start, priority is %d\n", task_prio_get (current));

    task_delay (10);
    /**
     * asynchronous processing
     */
    // 2)
    msg_cb_t cb;
    while (dummy < 5)
        {
        // mq_timedrecv (q0, &cb, sizeof (msg_cb_t), 2)
        if (mq_tryrecv (q0, &cb, sizeof (msg_cb_t)) == 0)
            {
            kprintf ("task D recv data from q0, data: %s\n", cb.packet);
            }
        else
            {
            task_delay (1);
            }
        }

    task_delay (1);

    kprintf ("task D end\n");
    quit ();
    }

/**
 * task:  A > B > C > D
 * mq:    q0
 *
 * app1: thread message communication
 *       1) A timed transmit data to mq, B recv from mq
 *              A --> q0  <- B
 *
 * app2: asynchronous processing
 *       1) A send cmd, B and C hand out cmd
 *                       -> B
 *                      /
 *              A --> q0
 *                      X
 *                       -> C
 *
 *       2) A B C send cmd, D handle
 *              A ->
 *                  X
 *              B --> q0  <-- D
 *                  /
 *              c ->
 *
 */
void test_mq_app_simple (void **state)
    {
    dummy = 0;
    q0 = mq_create (sizeof (msg_cb_t), 2, 0);

    ta = task_spawn ("a", 18, 0, 0x400, (int (*) (uintptr_t)) task_a, 0);
    tb = task_spawn ("b", 19, 0, 0x400, (int (*) (uintptr_t)) task_b, 0);
    tc = task_spawn ("c", 20, 0, 0x400, (int (*) (uintptr_t)) task_c, 0);
    td = task_spawn ("d", 21, 0, 0x400, (int (*) (uintptr_t)) task_d, 0);

    wait ();

    mq_delete (q0);
    }

/** test entry */

void test_mq (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_mq_create),
        unit_test (test_mq_delete),
        unit_test (test_mq_open),
        unit_test (test_mq_send),
        unit_test (test_mq_recv),

        unit_test (test_mq_app_simple),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_mq", test_mq, NULL, NULL, 0);