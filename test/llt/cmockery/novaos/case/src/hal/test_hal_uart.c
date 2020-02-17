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
#include "hal_uart.h"
#include "task.h"
#include <string.h>

/** test timer interfaces */

void test_hal_uart_open (void **state)
    {
    assert_int_equal (NULL, hal_uart_open (NULL));
    assert_int_equal (errno, ERRNO_HAL_UART_ILLEGAL_NAME);
    
    assert_true (hal_uart_open ("uart1"));
    assert_true (hal_uart_open ("uart1")); // repeated sucessfully
    }

void test_hal_uart_register (void **state)
    {
    //open uart device
    hal_uart_t * uart = hal_uart_open ("uart1");
    assert_true (uart);
    kprintf("%s\r\n",uart->name);
    }

void test_hal_uart_read (void **state)
    {
      // Invalid uart device test
    assert_int_equal (0, hal_uart_read (NULL, 0, 0));
    assert_int_equal (errno, ERRNO_HAL_UART_ILLEGAL_ID);
    
    // Invalid buffer test
    hal_uart_t * uart = hal_uart_open ("uart1");
    assert_true (uart);
    assert_int_equal (0, hal_uart_read (uart, NULL, 0));
    assert_int_equal (errno, ERRNO_HAL_UART_ILLEGAL_BUFF);
    }

void test_hal_uart_poll_read (void **state)
    {
    // Invalid uart device test
    assert_int_equal (0, hal_uart_poll_read (NULL, 0, 0));
    assert_int_equal (errno, ERRNO_HAL_UART_ILLEGAL_ID);
    
    // Invalid buffer test
    hal_uart_t * uart = hal_uart_open ("uart1");
    assert_true (uart);
    assert_int_equal (0, hal_uart_poll_read (uart, NULL, 0));
    assert_int_equal (errno, ERRNO_HAL_UART_ILLEGAL_BUFF);
    }

void test_hal_uart_write (void **state)
    {
    // Invalid uart device test
    assert_int_equal (0, hal_uart_write (NULL, 0, 0));
    assert_int_equal (errno, ERRNO_HAL_UART_ILLEGAL_ID);
    
    // Invalid buffer test
    hal_uart_t * uart = hal_uart_open ("uart1");
    assert_true (uart);
    assert_int_equal (0, hal_uart_write (uart, NULL, 0));
    assert_int_equal (errno, ERRNO_HAL_UART_ILLEGAL_BUFF);
    
    hal_uart_t * uart1 = hal_uart_open ("uart1");
    uart1->mode = HAL_UART_MODE_INT;
    unsigned char ptr2[] = "uart_write ok\r\n";
    hal_uart_write (uart1, ptr2, 16);
    }

void test_hal_uart_poll_write (void **state)
    {
    // Invalid uart device test
    assert_int_equal (0, hal_uart_poll_write (NULL, 0, 0));
    assert_int_equal (errno, ERRNO_HAL_UART_ILLEGAL_ID);
    
    // Invalid buffer test
    hal_uart_t * uart = hal_uart_open ("uart1");
    assert_true (uart);
    assert_int_equal (0, hal_uart_poll_write (uart, NULL, 0));
    assert_int_equal (errno, ERRNO_HAL_UART_ILLEGAL_BUFF);
    
    unsigned char ptr[] = "ok\r\n";
    hal_uart_poll_write (uart, ptr, 4);
    }

void test_hal_uart (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_hal_uart_open),
        unit_test (test_hal_uart_register),
        unit_test (test_hal_uart_read),
        unit_test (test_hal_uart_poll_read),
        unit_test (test_hal_uart_write),
        unit_test (test_hal_uart_poll_write),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_hal_uart", test_hal_uart, NULL, NULL, 0);
