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
#include "xprintf.h"
#include "task.h"

/** test timer interfaces */

static unsigned char addr [] = "hello world!";
static unsigned char str [] = "nova test";
static unsigned char buffer [1024];

static int _sprintf_putc(uintptr_t arg, unsigned char ch)
    {
    unsigned char **_buffer = (unsigned char **)arg;
    **_buffer = ch;
    (*_buffer)++;
    return 1;
    }

static int sprintf (unsigned char *string, char *format,...)
    {
    if (string == NULL)
        {
        return -1;
        }
    va_list valist;
    int     nbytes;
    va_start (valist, format);
    nbytes = xprintf (format, valist, _sprintf_putc, (uintptr_t)&string);
    va_end (valist);
    if (nbytes == 0)
        {
        return -1;
        }    
    return nbytes;
    }

static void setup (void)
{
    task_delay (5);
}

static void teardown (void)
{
    task_delay (5);
}

void test_sprintf(void **state)
    {
    assert_int_equal (13, sprintf (buffer, "%s\n", addr));
    assert_true (kprintf ("%s\n", buffer));
    assert_int_equal (10, sprintf (buffer, "%s\n", str));
    assert_true (kprintf ("%s\n", buffer));
    }

void test_kprintf_int (void **state)
    {
    assert_true (kprintf ("%d\n", 0xff));
    assert_true (kprintf ("%d\n", 7.4));
    assert_true (kprintf ("%d\n", 'a'));
    assert_true (kprintf ("%d\n", "abc"));
    assert_true (kprintf ("%d\n", addr));

    assert_int_equal (15, kprintf ("int=%d\n", 2147483647));
    assert_int_equal (16, kprintf ("int=%d\n", -2147483647));
    assert_int_equal (16, kprintf ("lint=%ld\n", 2147483647));
    assert_int_equal (17, kprintf ("lint=%ld\n", -2147483647));

    assert_int_equal (7, kprintf ("int=%d\n", 12));
    assert_int_equal (7, kprintf ("int=%*d\n", 1, 12));
    assert_int_equal (11, kprintf ("int=%*d\n", 6, 12));
    assert_int_equal (11, kprintf ("int=%+*d\n", 6, 12));
    assert_int_equal (11, kprintf ("int=%-*d\n", 6, 12));
    assert_int_equal (11, kprintf ("int=%0*d\n", 6, 12));
    assert_int_equal (11, kprintf ("int=%0+*d\n", 6, 12));
    assert_int_equal (11, kprintf ("int=%0-*d\n", 6, 12));

    assert_int_equal (7, kprintf ("int=%1d\n", 12));
    assert_int_equal (11, kprintf ("int=%6d\n", 12));
    assert_int_equal (11, kprintf ("int=%+6d\n", 12));
    assert_int_equal (11, kprintf ("int=%-6d\n", 12));
    assert_int_equal (11, kprintf ("int=%06d\n", 12));
    assert_int_equal (11, kprintf ("int=%0+6d\n", 12));
    assert_int_equal (11, kprintf ("int=%0-6d\n", 12));
    }

void test_kprintf_uint (void **state)
    {
    assert_true (kprintf ("%u\n", 'a'));
    assert_true (kprintf ("%u\n", "abc"));
    assert_true (kprintf ("%u\n", -12));
    assert_true (kprintf ("%u\n", 7.4));
    assert_true (kprintf ("%u\n", 0xff));
    assert_true (kprintf ("%u\n", addr)); 

    //assert_int_equal (17, kprintf ("\nuint=%lu\n", 4294967295));
    //assert_int_equal (16, kprintf ("uint=%u\n", 4294967295));
    //assert_int_equal (18, kprintf ("uint=%12u\n", 4294967295));
    //assert_int_equal (16, kprintf ("uint=%8u\n", 4294967295));
    //assert_int_equal (18, kprintf ("uint=%012u\n", 4294967295));
    //assert_int_equal (18, kprintf ("uint=%-12u\n", 4294967295));
    //assert_int_equal (18, kprintf ("uint=%-012u\n", 4294967295));
    //assert_int_equal (18, kprintf ("uint=%+12u\n", 4294967295));
    //assert_int_equal (18, kprintf ("uint=%+012u\n", 4294967295));
    assert_true (kprintf ("uint=%u\n", 2147483647));
    assert_true (kprintf ("uint=%u\n", 4294967295));
    
    assert_true (kprintf ("uint=%lu\n", 4294967295));
    assert_true (kprintf ("uint=%12u\n", 4294967295));
    assert_true (kprintf ("uint=%*u\n", 4294967295));
    assert_true (kprintf ("uint=%-*u\n", 4294967295));
    assert_true (kprintf ("uint=%+*u\n", 4294967295));

    assert_int_equal (18, kprintf ("uint=%*u\n", 12, 4294967295));
    assert_int_equal (16, kprintf ("uint=%*u\n", 8, 4294967295));
    assert_int_equal (18, kprintf ("uint=%0*u\n", 12, 4294967295));
    assert_int_equal (18, kprintf ("uint=%-*u\n", 12, 4294967295));
    assert_int_equal (18, kprintf ("uint=%-0*u\n", 12, 4294967295));
    assert_int_equal (18, kprintf ("uint=%+*u\n", 12, 4294967295));
    assert_int_equal (18, kprintf ("uint=%+0*u\n", 12, 4294967295));
    }

void test_kprintf_hex (void **state)
    {
    assert_true (kprintf ("%x\n", 'a'));
    assert_true (kprintf ("%x\n", "abc"));
    assert_true (kprintf ("%x\n", 12));
    assert_true (kprintf ("%x\n", -12));
    assert_true (kprintf ("%x\n", 7.4));
    assert_true (kprintf ("%x\n", addr));
    assert_int_equal (13, kprintf ("hex=%x\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%12x\n", 0xffffffff));
    assert_int_equal (13, kprintf ("hex=%8x\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%012x\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%-12x\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%-012x\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%+12x\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%+012x\n", 0xffffffff));

    assert_int_equal (17, kprintf ("hex=%*x\n", 12, 0xffffffff));
    assert_int_equal (13, kprintf ("hex=%*x\n", 8, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%0*x\n", 12, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%-*x\n", 12, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%-0*x\n", 12, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%+*x\n", 12, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%+0*x\n", 12, 0xffffffff));

    assert_true (kprintf ("%X\n", 'a'));
    assert_true (kprintf ("%X\n", "abc"));
    assert_true (kprintf ("%X\n", 12));
    assert_true (kprintf ("%X\n", -12));
    assert_true (kprintf ("%X\n", 7.4));
    assert_true (kprintf ("%X\n", addr));
    assert_int_equal (13, kprintf ("hex=%X\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%12X\n", 0xffffffff));
    assert_int_equal (13, kprintf ("hex=%8X\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%012X\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%-12X\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%-012X\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%+12X\n", 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%+012X\n", 0xffffffff));

    assert_int_equal (17, kprintf ("hex=%*X\n", 12, 0xffffffff));
    assert_int_equal (13, kprintf ("hex=%*X\n", 8, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%0*X\n", 12, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%-*X\n", 12, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%-0*X\n", 12, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%+*X\n", 12, 0xffffffff));
    assert_int_equal (17, kprintf ("hex=%+0*X\n", 12, 0xffffffff));
    }

void test_kprintf_point (void **state)
    {
    assert_true (kprintf ("%p\n", 'a'));
    assert_true (kprintf ("%p\n", "abc"));
    assert_true (kprintf ("%p\n", 0xff));
    assert_true (kprintf ("%p\n", 12));
    assert_true (kprintf ("%p\n", -12));
    assert_true (kprintf ("%p\n", 7.4));

    assert_int_equal (16, kprintf ("addr=%p\n", addr));
    assert_int_equal (18, kprintf ("addr=%12p\n", addr));
    assert_int_equal (16, kprintf ("addr=%8p\n", addr));
    assert_int_equal (18, kprintf ("addr=%012p\n", addr));
    assert_int_equal (18, kprintf ("addr=%-12p\n", addr));
    assert_int_equal (18, kprintf ("addr=%-012p\n", addr));
    assert_int_equal (18, kprintf ("addr=%+12p\n", addr));
    assert_int_equal (18, kprintf ("addr=%+012p\n", addr));

    assert_int_equal (18, kprintf ("addr=%*p\n", 12, addr));
    assert_int_equal (16, kprintf ("addr=%*p\n", 8, addr));
    assert_int_equal (18, kprintf ("addr=%0*p\n", 12, addr));
    assert_int_equal (18, kprintf ("addr=%-*p\n", 12, addr));
    assert_int_equal (18, kprintf ("addr=%-0*p\n", 12, addr));
    assert_int_equal (18, kprintf ("addr=%+*p\n", 12, addr));
    assert_int_equal (18, kprintf ("addr=%+0*p\n", 12, addr));
    }

void test_kprintf_char (void **state)
    {
    assert_true (kprintf ("%c\n", "abc"));
    assert_true (kprintf ("%c\n", 0xff));
    assert_true (kprintf ("%c\n", 12));
    assert_true (kprintf ("%c\n", -12));
    assert_true (kprintf ("%c\n", addr));
    assert_true (kprintf ("%c\n", 7.4));

    assert_int_equal (2, kprintf ("%%\n"));

    assert_int_equal (7, kprintf ("char=%c\n", 'a'));
    assert_int_equal (12, kprintf ("char=%6c\n", 'a'));
    assert_int_equal (12, kprintf ("char=%06c\n", 'a'));
    assert_int_equal (12, kprintf ("char=%-6c\n", 'a'));
    assert_int_equal (12, kprintf ("char=%-06c\n", 'a'));
    assert_int_equal (12, kprintf ("char=%+6c\n", 'a'));
    assert_int_equal (12, kprintf ("char=%+06c\n", 'a'));

    assert_int_equal (12, kprintf ("char=%*c\n", 6, 'a'));
    assert_int_equal (12, kprintf ("char=%0*c\n", 6, 'a'));
    assert_int_equal (12, kprintf ("char=%-*c\n", 6, 'a'));
    assert_int_equal (12, kprintf ("char=%-0*c\n", 6, 'a'));
    assert_int_equal (12, kprintf ("char=%+*c\n", 6, 'a'));
    assert_int_equal (12, kprintf ("char=%+0*c\n", 6, 'a'));
    }

void test_kprintf_string (void **state)
    {
    assert_true (kprintf ("%s\n", 'a'));
    assert_true (kprintf ("%s\n", 0xff));
    assert_true (kprintf ("%s\n", 12));
    assert_true (kprintf ("%s\n", -12));
    assert_true (kprintf ("%s\n", 7.4));

    assert_int_equal (20, kprintf ("string=%s\n", addr));
    assert_int_equal (22, kprintf ("string=%14s\n", addr));
    assert_int_equal (20, kprintf ("string=%10s\n", addr));
    assert_int_equal (22, kprintf ("string=%014s\n", addr));
    assert_int_equal (22, kprintf ("string=%-14s\n", addr));
    assert_int_equal (22, kprintf ("string=%-014s\n", addr));
    assert_int_equal (22, kprintf ("string=%+14s\n", addr));
    assert_int_equal (22, kprintf ("string=%+014s\n", addr));

    assert_int_equal (22, kprintf ("string=%*s\n", 14, addr));
    assert_int_equal (20, kprintf ("string=%*s\n", 10, addr));
    assert_int_equal (22, kprintf ("string=%0*s\n", 14, addr));
    assert_int_equal (22, kprintf ("string=%-*s\n", 14, addr));
    assert_int_equal (22, kprintf ("string=%-0*s\n", 14, addr));
    assert_int_equal (22, kprintf ("string=%+*s\n", 14, addr));
    assert_int_equal (22, kprintf ("string=%+0*s\n", 14, addr));

    assert_int_equal (70, kprintf ("string=%s\n", "0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));
    assert_int_equal (37, kprintf ("string=%s\n", "`~!@#$%^&*()_+-={}|[]:;<>?,./"));  //lack \ "'

    assert_int_equal (10, kprintf ("string=%s\n", "ab"));
    assert_int_equal (14, kprintf ("string=%6s\n", "ab"));
    assert_int_equal (10, kprintf ("string=%1s\n", "ab"));
    assert_int_equal (14, kprintf ("string=%06s\n", "ab"));
    assert_int_equal (14, kprintf ("string=%-6s\n", "ab"));
    assert_int_equal (14, kprintf ("string=%-06s\n", "ab"));
    assert_int_equal (14, kprintf ("string=%+6s\n", "ab"));
    assert_int_equal (14, kprintf ("string=%+06s\n", "ab"));

    assert_int_equal (14, kprintf ("string=%*s\n", 6, "ab"));
    assert_int_equal (10, kprintf ("string=%*s\n", 1, "ab"));
    assert_int_equal (14, kprintf ("string=%0*s\n", 6, "ab"));
    assert_int_equal (14, kprintf ("string=%-*s\n", 6, "ab"));
    assert_int_equal (14, kprintf ("string=%-0*s\n", 6, "ab"));
    assert_int_equal (14, kprintf ("string=%+*s\n", 6, "ab"));
    assert_int_equal (14, kprintf ("string=%+0*s\n", 6, "ab"));
    }

/** test entry */

void test_kprintf (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_sprintf),
        unit_test (test_kprintf_int),
        unit_test (test_kprintf_uint),
        unit_test (test_kprintf_hex),
        unit_test (test_kprintf_point),
        unit_test (test_kprintf_char),
        unit_test (test_kprintf_string),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_kprintf", test_kprintf, setup, teardown, 0);
