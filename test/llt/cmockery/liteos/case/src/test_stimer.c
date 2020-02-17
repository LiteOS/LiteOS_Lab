/*----------------------------------------------------------------------------
 * Copyright (c) <2020>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
#include "tester.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "cmockery.h"

#include <stddef.h>
#include <stdint.h>
#include <stimer.h>
#include <stdio.h>

static void stimer_handler(void *args)
{
    const char *timer_name = args;

    printf("timer:%s triggered ---systime:%d\n\r",timer_name,(uint32_t)osal_sys_time());

    return;
}

static void test_stimer_create (void **state)
{
    stimer_t s_stimer_t;
    s_stimer_t = stimer_create ("stimer1", NULL, "stimer1", 100, cn_stimer_flag_start);
    assert_int_not_equal (NULL, s_stimer_t);
    stimer_delete (s_stimer_t);
}

static void test_stimer_delete (void **state)
{
    stimer_t s_stimer_t;
    s_stimer_t = stimer_create ("stimer1", NULL, "stimer1", 100, cn_stimer_flag_start);
    assert_int_not_equal (NULL, s_stimer_t);
    assert_int_equal (0, stimer_delete (s_stimer_t));
}

static void test_stimer_ioctl (void **state)
{
    assert_int_equal (-1, stimer_ioctl (NULL, 1, NULL));

    stimer_t s_stimer_t;
    s_stimer_t = stimer_create ("stimer1", NULL, "stimer1", 100, cn_stimer_flag_start);
    assert_int_not_equal (NULL, s_stimer_t);
    assert_int_equal (0, stimer_ioctl (s_stimer_t, 1, NULL));
    assert_int_equal (0, stimer_delete (s_stimer_t));
}

static void test_stimer_demo (void **state)
{
    stimer_t s_stimer_t1;
    stimer_t s_stimer_t2;
    s_stimer_t1 = stimer_create("stimer1",stimer_handler,"stimer1",300,cn_stimer_flag_start);
    s_stimer_t2 = stimer_create("stimer2",stimer_handler,"stimer6",200,cn_stimer_flag_start|cn_stimer_flag_once);
    osal_task_sleep(2*1000);
    stimer_delete (s_stimer_t1);
    stimer_delete (s_stimer_t2);
}


void test_stimer (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_stimer_create),
        unit_test (test_stimer_delete),
        unit_test (test_stimer_ioctl),
        unit_test (test_stimer_demo),
        };

    stimer_init();

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_stimer", test_stimer, NULL, NULL, 0);