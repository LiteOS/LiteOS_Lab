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
#include "hal_timer.h"
#include "task.h"
#include "timer.h"

// hal_timer initialization
#define MAX_COUNT  10
static timer_id t1_id;
static int _test_enable (hal_timer_t * timer, unsigned long count)
    {
    timer->max_count = count; 
    t1_id = timer_create (0, count, timer->handler, timer->arg);
    return timer_start (t1_id);
    }

static int _test_disable (hal_timer_t *timer)
    {
    timer_stop (t1_id);
    return timer_delete (t1_id);
    }

static void _test_postpone (hal_timer_t *timer, unsigned long count)
    {
    unsigned long post_val = (timer->max_count) + count;
    timer->max_count = post_val;
    }

static void _test_prepone (hal_timer_t *timer, unsigned long count)
    {
    unsigned long pre_val = (timer->max_count) - count;
    timer->max_count = pre_val;
    }

static unsigned long _test_counter (hal_timer_t *timer)
    {
    // TO DO
    return -1;
    }

static unsigned long _test_period (hal_timer_t *timer)
    {
    return timer->max_count;
    }


static hal_timer_methods_t test_methods =
    {
    .enable   = _test_enable,
    .disable  = _test_disable,
    .postpone = _test_postpone,
    .prepone  = _test_prepone,
    .counter  = _test_counter,
    .period   = _test_period,
    };

static void _test_hal_timer_init (hal_timer_t *timer, const char *name)
    {
    timer->name    =    name;
    timer->freq    =    115200;
    timer->max_count =  10;
    timer->methods   =  &test_methods;
    }

static void handler_task (uintptr_t arg)
    {
    (void) arg;
    task_delay (10);
    kprintf ("test_hal_handler sucess!");
    }

/** test timer interfaces */
void test_hal_timer_register (void **state)
    {
    hal_timer_t test_timer;
    _test_hal_timer_init (&test_timer, "test_timer1");
    hal_timer_register (&test_timer);
    }

void test_hal_timer_connect (void **state)
    {
    assert_int_equal (-1, hal_timer_connect (NULL, handler_task, 0));
    assert_int_equal (errno, ERRNO_HAL_TIMER_ILLEGAL_ID);

    hal_timer_t connect_test_timer1;
    _test_hal_timer_init (&connect_test_timer1, "test_timer1");
    hal_timer_register (&connect_test_timer1);
    assert_int_equal (0, hal_timer_connect (&connect_test_timer1, handler_task, 0));
    }

void test_hal_timer_enable (void **state)
    {
    assert_int_equal (-1, hal_timer_enable (NULL, HAL_TIMER_MODE_ONE_SHOT, 5));
    assert_int_equal (errno, ERRNO_HAL_TIMER_ILLEGAL_ID);
    
    hal_timer_t enable_test_timer1;
    _test_hal_timer_init (&enable_test_timer1, "test_timer1");
    hal_timer_register (&enable_test_timer1);
    assert_int_equal (0, hal_timer_connect (&enable_test_timer1, handler_task, 0));
    assert_int_equal (-1, hal_timer_enable (&enable_test_timer1, HAL_TIMER_MODE_ONE_SHOT, 11));
    assert_int_equal (errno, ERRNO_HAL_TIMER_ILLEGAL_RANGE);
    assert_int_equal (0, hal_timer_enable (&enable_test_timer1, HAL_TIMER_MODE_ONE_SHOT, 5));
    assert_int_equal (0, hal_timer_enable (&enable_test_timer1, HAL_TIMER_MODE_ONE_SHOT, 5)); // repeated enable is ok
    assert_int_equal (0, hal_timer_disable (&enable_test_timer1));
    }

void test_hal_timer_disable (void **state)
    {
    assert_int_equal (-1, hal_timer_disable (NULL));
    assert_int_equal (errno, ERRNO_HAL_TIMER_ILLEGAL_ID);

    hal_timer_t disable_test_timer1;
    _test_hal_timer_init (&disable_test_timer1, "test_timer1");
    hal_timer_register (&disable_test_timer1);
    assert_int_equal (0, hal_timer_connect (&disable_test_timer1, handler_task, 0));
    assert_int_equal (0, hal_timer_enable (&disable_test_timer1, HAL_TIMER_MODE_ONE_SHOT, 5));
    assert_int_equal (0, hal_timer_disable (&disable_test_timer1));
    assert_int_equal (-1, hal_timer_disable (&disable_test_timer1)); // repeated
    }

void test_hal_timer_feat_get (void **state)
    {
    static uint32_t *get_max_count;
    static uint32_t *get_freq;
    assert_int_equal (-1, hal_timer_feat_get (NULL, get_max_count, get_freq));
    assert_int_equal (errno, ERRNO_HAL_TIMER_ILLEGAL_ID);

    hal_timer_t feat_get_test_timer1;
    _test_hal_timer_init (&feat_get_test_timer1, "test_timer1");
    hal_timer_register (&feat_get_test_timer1);
    assert_int_equal (0, hal_timer_connect (&feat_get_test_timer1, handler_task, 0));
    assert_int_equal (0, hal_timer_enable (&feat_get_test_timer1, HAL_TIMER_MODE_ONE_SHOT, 5));

    assert_int_equal (0, hal_timer_feat_get (&feat_get_test_timer1, get_max_count, get_freq));
    kprintf ("max_count = %d\n", &get_max_count);
    kprintf ("freq= %d\n", &get_freq);
    assert_int_equal (0, hal_timer_disable (&feat_get_test_timer1));
    }

void test_hal_timer_period (void **state)
    {
    assert_int_equal (0, hal_timer_period (NULL));
    assert_int_equal (errno, errno = ERRNO_HAL_TIMER_ILLEGAL_ID);
    
    hal_timer_t period_test_timer1;
    _test_hal_timer_init (&period_test_timer1, "test_timer1");
    hal_timer_register (&period_test_timer1);
    assert_int_equal (0, hal_timer_connect (&period_test_timer1, handler_task, 0));
    assert_int_equal (0, hal_timer_enable (&period_test_timer1, HAL_TIMER_MODE_ONE_SHOT, 5));
    assert_true (hal_timer_period (&period_test_timer1));
    assert_int_equal (0, hal_timer_disable (&period_test_timer1));
    }

void test_hal_timer_postpone (void **state)
    {
    hal_timer_t postpone_test_timer1;
    _test_hal_timer_init (&postpone_test_timer1, "test_timer1");
    hal_timer_register (&postpone_test_timer1);
    assert_int_equal (0, hal_timer_connect (&postpone_test_timer1, handler_task, 0));
    assert_int_equal (0, hal_timer_enable (&postpone_test_timer1, HAL_TIMER_MODE_ONE_SHOT, 5));

    assert_int_equal (-1, hal_timer_postpone (NULL, 2));
    assert_int_equal (errno, errno = ERRNO_HAL_TIMER_ILLEGAL_ID);
    assert_int_equal (0, hal_timer_postpone (&postpone_test_timer1, 3));
    assert_int_equal (0, hal_timer_disable (&postpone_test_timer1));
    }

void test_hal_timer_prepone (void **state)
    {
    hal_timer_t prepone_test_timer1;
    _test_hal_timer_init (&prepone_test_timer1, "test_timer1");
    hal_timer_register (&prepone_test_timer1);
    assert_int_equal (0, hal_timer_connect (&prepone_test_timer1, handler_task, 0));
    assert_int_equal (0, hal_timer_enable (&prepone_test_timer1, HAL_TIMER_MODE_ONE_SHOT, 5));

    assert_int_equal (-1, hal_timer_postpone (NULL, 2));
    assert_int_equal (errno, errno = ERRNO_HAL_TIMER_ILLEGAL_ID);
    assert_int_equal (0, hal_timer_postpone (&prepone_test_timer1, 3));
    assert_int_equal (0, hal_timer_disable (&prepone_test_timer1));
    }


void test_hal_timer (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_hal_timer_register),
        unit_test (test_hal_timer_connect),
        unit_test (test_hal_timer_enable),
        unit_test (test_hal_timer_disable),
        unit_test (test_hal_timer_feat_get),
        unit_test (test_hal_timer_period),
        unit_test (test_hal_timer_postpone),
        unit_test (test_hal_timer_prepone),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_hal_timer", test_hal_timer, NULL, NULL, 0);