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

#include <stdlib.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "cmockery.h"

#include "kprintf.h"
#include "task.h"
#include "sem.h"
#include "mempool.h"

#define TEST_ARRAY_SIZE         32

static uint64_t pool_buff [100];
static mempool_id test_pool;
static char * test_array [TEST_ARRAY_SIZE] = { NULL };
static sem_t sem;
static volatile bool test_exit = false;

void test_mempool_create (void **state)
    {
    mempool_id m_id;

    assert_false (mempool_create (0, 0, NULL));
    assert_int_equal (errno, ERRNO_MEMPOOL_ILLEGAL_PARA);
    assert_false (mempool_create (8, 0, NULL));
    assert_int_equal (errno, ERRNO_MEMPOOL_ILLEGAL_PARA);
    assert_false (mempool_create (10, 1, NULL));
    assert_int_equal (errno, ERRNO_MEMPOOL_ILLEGAL_PARA);

    assert_true (m_id = mempool_create (8, 10, (char *)pool_buff));
    assert_int_equal (0, mempool_destroy (m_id));
    }

void test_mempool_destroy (void **state)
    {
    mempool_t m;

    assert_int_equal (-1, mempool_destroy (NULL));
    assert_int_equal (errno, ERRNO_MEMPOOL_ILLEGAL_ID);
    assert_int_equal (-1, mempool_destroy (&m));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    assert_int_equal (-1, mempool_destroy ((mempool_id)0x1234)); // invalid address
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    }

void test_mempool_alloc (void **state)
    {
    mempool_t m;

    assert_false (mempool_alloc (NULL));
    assert_int_equal (errno, ERRNO_MEMPOOL_ILLEGAL_ID);
    assert_false (mempool_alloc (&m));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    assert_false (mempool_alloc ((mempool_id)0x1234)); // invalid address
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

void test_mempool_free (void **state)
    {
    mempool_t m;

    assert_int_equal (-1, mempool_free (NULL, NULL));
    assert_int_equal (errno, ERRNO_MEMPOOL_ILLEGAL_ID);
    assert_int_equal (-1, mempool_free (&m, NULL));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    assert_int_equal (-1, mempool_free ((mempool_id)0x1234, NULL)); // invalid address
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    }

static int __task_malloc (uintptr_t arg)
    {
    int i;

    for (i = 0; i < 50; i++)
        {
        int idx = rand () & (TEST_ARRAY_SIZE - 1);

        if (test_array [idx] == NULL)
            {
            test_array [idx] = mempool_alloc (test_pool);
            kprintf ("task malloc, index %d\n", idx);
            }
        // else
        //     {
        //     assert_int_equal (0, mempool_free (test_pool, test_array [idx]));
        //     test_array [idx] = NULL;
        //     }
        }

    sem_post (&sem);
    test_exit = true;

    return 0;
    }

static int __task_free (uintptr_t arg)
    {
    while (!test_exit)
        {
        int i;
        for (i = 0; i < TEST_ARRAY_SIZE; i++)
            {
            if (test_array [i] != NULL)
                {
                kprintf ("task free,   index %d\n", i);
                assert_int_equal (0, mempool_free (test_pool, test_array [i]));
                test_array [i] = NULL;
                break;
                }
            }

        task_delay (2);
        }

    sem_post (&sem);

    return 0;
    }

void test_mempool_demo (void **state)
    {
    int i;
    sem_init (&sem, 0);
    test_exit = false;

    assert_true (test_pool = mempool_create (8, 10, (char *)pool_buff));

    task_delay (5);

    task_spawn ("tm", 18, 0, 0x400, (int (*) (uintptr_t)) __task_malloc, 0);
    task_spawn ("tf", 19, 0, 0x400, (int (*) (uintptr_t)) __task_free, 0);

    sem_wait (&sem);
    sem_wait (&sem);

    task_delay (5);

    for (i = 0; i < TEST_ARRAY_SIZE; i++)
        {
        if (test_array [i] != NULL)
            {
            assert_int_equal (0, mempool_free (test_pool, test_array [i]));
            test_array [i] = NULL;
            }
        }

    assert_int_equal (0, mempool_destroy (test_pool));

    sem_destroy (&sem);

    task_delay (5);
    }

/** test entry */

void test_mempool (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_mempool_create),
        unit_test (test_mempool_destroy),
        unit_test (test_mempool_alloc),
        unit_test (test_mempool_free),

        unit_test (test_mempool_demo),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_mempool", test_mempool, NULL, NULL, 0);