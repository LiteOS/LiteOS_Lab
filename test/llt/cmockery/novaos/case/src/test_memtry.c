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
#include "sem.h"
#include "memtry.h"

void test_mem_try (void **state)
    {
    uint32_t value;
    int s = 0;
    int f = 0;
    int i;
    char *p = NULL;

    assert_int_equal (-1, mem_try (NULL, NULL, MEMTRY_MAX_ORDER + 1));
    assert_int_equal (errno, ERRNO_MEMTRY_ILLEGAL_ORDER);

    // assert_int_equal (-1, mem_try (NULL, NULL, 2));
    for (i = 0; i < 4096; i++)
        {
        if (mem_try (&value, p + (i << 20), 2) == -1)
            {
            assert_int_equal (-1, mem_try (p + (i << 20), &value, 2));
            f++;
            }
        else
            {
            s++;
            }
        }
    kprintf ("success: %d, fail: %d\n\n", s, f);
    }

static sem_t sem;

static void __timer_once (uintptr_t arg)
    {
    uint8_t  d0 = 0;
    uint16_t d1 = 0;
    uint32_t d2 = 0;
    uint64_t d3 = 0;

    uint8_t  s0 = 0x12;
    uint16_t s1 = 0x1234;
    uint32_t s2 = 0x12345678;
    uint64_t s3 = 0x12345678abcdefff;

    (void) arg;

    assert_int_equal (0, mem_try (&d0, &s0, 0));
    assert_int_equal (d0, s0);
    assert_int_equal (0, mem_try (&d1, &s1, 1));
    assert_int_equal (d1, s1);
    assert_int_equal (0, mem_try (&d2, &s2, 2));
    assert_int_equal (d2, s2);
    assert_int_equal (0, mem_try (&d3, &s3, 3));
    assert_int_equal (d3, s3);

    d2 = d1 = d0 = 0;
    d3 = 0;
    kprintf ("\nd0=%d, d1=%d, d2=%d, d3=0x%llx\n\n", d0, d1, d2, d3);

    assert_int_equal (0, mem_try (&d0, &s1, 0));
    kprintf ("s1: 0x%x, d0: 0x%x\n", s1, d0);
    assert_int_equal (0, mem_try (&d1, &s2, 1));
    kprintf ("s2: 0x%x, d1: 0x%x\n", s2, d1);
    assert_int_equal (0, mem_try (&d2, &s3, 2));
    kprintf ("s3: 0x%llx, d2: 0x%x\n", s3, d2);

    kprintf ("\n");

    d2 = d1 = d0 = 0;
    d3 = 0;
    kprintf ("d0=%d, d1=%d, d2=%d, d3=0x%llx\n\n", d0, d1, d2, d3);

    assert_int_equal (0, mem_try (&d3, &s2, 3));
    kprintf ("s2: 0x%x, d3: 0x%llx\n", s2, d3);
    kprintf ("s2 next: 0x%x\n", (uint32_t)(*(&s2 + 1)));
    assert_int_equal (0, mem_try (&d2, &s1, 2));
    kprintf ("s1: 0x%x, d2: 0x%x\n", s1, d2);
    kprintf ("s1 next: 0x%x\n", (uint16_t)(*(&s1 + 1)));
    assert_int_equal (0, mem_try (&d1, &s0, 1));
    kprintf ("s0: 0x%x, d1: 0x%x\n", s0, d1);
    kprintf ("s0 next: 0x%x\n", (uint8_t)(*(&s0 + 1)));
    assert_int_equal (0, mem_try (&d0, &s3, 0));
    kprintf ("s3: 0x%llx, d0: 0x%x\n", s3, d0);

    kprintf ("\n");

    sem_post (&sem);
    }

void test_mem_try_demo (void **state)
    {
    timer_t tim1;

    sem_init (&sem, 0);

    /* call the test case in task context */

    kprintf ("\n>>>> test mem_try in task context\n");

    __timer_once (0);

    /* eat the post ... */

    sem_wait (&sem);

    /* call the test case in IRQ context */

    kprintf (">>>> test mem_try in IRQ context\n");

    timer_init (&tim1, 0, 3, __timer_once, 0x1234);

    timer_start (&tim1);

    sem_wait (&sem);

    sem_destroy (&sem);

    timer_delete (&tim1);
    }
/** test entry */

void test_memtry (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_mem_try),

        unit_test (test_mem_try_demo),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_memtry", test_memtry, NULL, NULL, 0);