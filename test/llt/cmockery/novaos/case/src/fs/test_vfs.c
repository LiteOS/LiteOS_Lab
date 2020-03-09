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
#include "vfs.h"
#include "task.h"

/** test timer interfaces */

void test_vfs_open (void **state)
    {
    assert_int_equal (-1, vfs_open (NULL, O_RDWR));
    assert_int_equal (-1, vfs_open ("", O_RDWR));
    assert_int_equal (-1, vfs_open ("/", O_RDWR));
    assert_int_equal (-1, vfs_open ("/dev/", O_RDWR));
    
    int fd = vfs_open ("/dev/uart1", O_RDWR);
    assert_int_not_equal (-1, fd);
    assert_int_equal (0, vfs_close(fd));
    }

void test_vfs_close (void **state)
    {
    assert_int_equal (-1, vfs_close (-1));
    assert_int_equal (-1, vfs_close (CONFIG_MAX_NR_FILES+1));

    int fd = vfs_open ("/dev/uart1", O_RDWR);   
    assert_int_equal (0, vfs_close(fd));
    kprintf ("vfs_close test !\r\n");
    }

void test_vfs_read (void **state)
    {
    assert_int_equal (-1, vfs_read (-1, 0, 0));
    assert_int_equal (-1, vfs_read (CONFIG_MAX_NR_FILES, 0, 0));
    /*
    uint8_t buff [2];
    int fd = vfs_open ("/dev/uart1", O_RDWR);
    assert_true (vfs_read (fd, (char *) &buff, 2));
    assert_int_equal (0, vfs_close(fd));
    */
    }

void test_vfs_write (void **state)
    {
    assert_int_equal (-1, vfs_write (-1, "ok", strlen("ok")));
    assert_int_equal (-1, vfs_write (CONFIG_MAX_NR_FILES, "ok", strlen ("ok")));

    int fd = vfs_open ("/dev/uart1", O_RDWR);   
    assert_true (vfs_write (fd, "vfs_write test OK!\r\n", strlen ("vfs_write test OK!\r\n")));
    assert_int_equal (0, vfs_close(fd));
    }

void test_vfs_lseek (void **state)
    {
    assert_int_equal (-1, vfs_lseek (-1, 5, 5));
    assert_int_equal (-1, vfs_lseek (CONFIG_MAX_NR_FILES, 5, 5));
    }

void test_vfs_ioctl (void **state)
    {
    assert_int_equal (-1, vfs_ioctl (-1, 5));
    assert_int_equal (-1, vfs_ioctl (CONFIG_MAX_NR_FILES, 5));
    }

void test_vfs (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_vfs_open),
        unit_test (test_vfs_close),
        unit_test (test_vfs_read),
        unit_test (test_vfs_write),
        unit_test (test_vfs_lseek),
        unit_test (test_vfs_ioctl),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_vfs", test_vfs, NULL, NULL, 0);