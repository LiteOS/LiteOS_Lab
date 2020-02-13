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

#include <driver.h>
#include <sys/fcntl.h>

#define cn_testdriv_buf_len 256

/*-------------------------------------------------------------------------------------*/
typedef struct
{
    int refers;
}testdriv_cb_test_t;

static testdriv_cb_test_t  s_testdriv_cb_test;

//cached only one frame here
static bool_t testdriv_open(void *pri,int flag) // test_dir_open
{
    printf("TESTDRIV:PRI:0x%08X OPEN\n\r",(unsigned int)pri);
    s_testdriv_cb_test.refers++;
    return true;
}

static void testdriv_close(void *pri) // test_dir_close
{
    printf("TESTDRIV:PRI:0x%08X CLOSE\n\r",(unsigned int)pri);
    s_testdriv_cb_test.refers--;
    return ;
}

static bool_t testdriv_write(void *pri,unsigned int offset,unsigned char *buf,int len,unsigned int timeout) // test_dir_write
{
    printf("TESTDRIV:PRI:0x%08X WTRITE: buf:0x%08x len:%d timeout:%d\n\r",(unsigned int)pri,(unsigned int)buf,len,timeout);
    return len;
}

static bool_t testdriv_read(void *pri,unsigned int offset,unsigned char *buf,int len,unsigned int timeout) // test_dir_read
{
    printf("TESTDRIV:PRI:0x%08X READ: buf:0x%08x len:%d timeout:%d\n\r",(unsigned int)pri,(unsigned int)buf,len,timeout);
    return len;
}


static bool_t testdriv_init(void *pri) // test_dir_init
{
    printf("TESTDRIV:PRI:0x%08X INIT\n\r",(unsigned int)pri);
    return true;
}

static void testdriv_deinit(void *pri) // test_dir_deinit
{
    printf("TESTDRIV:PRI:0x%08X DEINIT\n\r",(unsigned int)pri);
    return ;
}

static bool_t testdriv_ioctl(void *pri,unsigned int cmd, void *para,int paralen) // test_dir_ioctl
{
    printf("TESTDRIV:PRI:0x%08X IOCTL:cmd:%d para:0x%08x paralen:%d \n\r",(unsigned int)pri,cmd,(unsigned int)para,paralen);
    return  true;
}

static const  los_driv_op_t  s_testdriv =
{
    .open = testdriv_open,
    .close = testdriv_close,
    .ioctl= testdriv_ioctl,
    .read = testdriv_read,
    .write = testdriv_write,
    .init = testdriv_init,
    .deinit = testdriv_deinit,
};

OSDRIV_EXPORT(drivpara1,"dev1",(los_driv_op_t *)&s_testdriv,NULL,O_RDWR);
OSDRIV_EXPORT(drivpara2,"dev2",(los_driv_op_t *)&s_testdriv,NULL,O_RDWR);
OSDRIV_EXPORT(drivpara3,"dev3",(los_driv_op_t *)&s_testdriv,NULL,O_RDWR);
//test device info
OSDRIV_EXPORT(drivpara4,NULL,(los_driv_op_t *)&s_testdriv,NULL,O_RDWR);
OSDRIV_EXPORT(drivpara5,"dev5",NULL,NULL,O_RDWR);
/*---------------------register and  unregister---------------------------------------------------*/
static void setup (void)
{
    los_driv_register (&drivpara1);
    los_driv_register (&drivpara2);
    los_driv_register (&drivpara3);
}


static void teardown (void)
{
    los_driv_unregister ("dev1");
    los_driv_unregister ("dev2");
    los_driv_unregister ("dev3");
}

/*-----------------------test_cases---------------------------------------------------------------*/
static void test_los_dev_register (void **state)
{
    assert_int_equal (NULL, los_driv_register (&drivpara4));
    assert_int_equal (NULL, los_driv_register (&drivpara5));
}

static void test_los_dev_init (void **state)
{
    assert_int_not_equal (0, los_driv_init ());
}

static void test_los_dev_open (void **state)
{
    //wrong parameter
    assert_int_equal (NULL, los_dev_open(NULL, O_RDWR));
    assert_int_equal (NULL, los_dev_open("dev6", O_RDWR));
    assert_int_equal (NULL, los_dev_open("dev7", O_RDWR));

    //correct parameter
    static los_dev_t dev_t;
    dev_t = los_dev_open ("dev1", O_RDWR);
    assert_int_not_equal (NULL, dev_t);
    assert_int_not_equal(NULL, los_dev_open ("dev1", O_RDWR)); //repeat open the same dev;
    los_dev_close (dev_t);
}

static void test_los_dev_close (void **state)
{
    assert_int_equal (0, los_dev_close (NULL));

    static los_dev_t dev_t;
    dev_t = los_dev_open ("dev2", O_RDWR);
    assert_int_equal (1, los_dev_close (dev_t));
    assert_int_equal (0, los_dev_close (dev_t)); // repeat close the same dev will be failed;
}

static void test_los_dev_read (void **state)
{
    ssize_t ret;
    void *buf;
    static los_dev_t dev_t;

    dev_t = los_dev_open ("dev2", O_RDWR);
    assert_int_not_equal (NULL, dev_t);

    buf = osal_malloc(128);
    assert_int_equal (0, los_dev_read (NULL, 0, buf, 128, 1000));
    assert_int_equal (0, los_dev_read (dev_t, 0, NULL, 128, 1000));
    assert_int_equal (0, los_dev_read (dev_t, 0, buf, 0, 1000));
    ret = los_dev_read(dev_t, 0, buf, 128, 1000);
    assert_true (ret);

    assert_int_equal (1, los_dev_close (dev_t));
}

static void test_los_dev_write (void **state)
{
    ssize_t ret;
    void *buf;
    static los_dev_t dev_t;

    dev_t = los_dev_open ("dev2", O_RDWR);
    assert_int_not_equal (NULL, dev_t);

    buf = osal_malloc(128);
    assert_int_equal (0, los_dev_write (NULL, 0, buf, 128, 1000));
    assert_int_equal (0, los_dev_write (dev_t, 0, NULL, 128, 1000));
    assert_int_equal (0, los_dev_write (dev_t, 0, buf, 0, 1000));

    ret = los_dev_write (dev_t, 0, buf, 128, 1000);
    assert_true (ret);

    assert_int_equal (1, los_dev_close (dev_t));
}

static void test_los_dev_ioctl (void **state)
{
    static los_dev_t dev_t;

    dev_t = los_dev_open ("dev2", O_RDWR);
    assert_int_not_equal (NULL, dev_t);

    //wrong parameter
    assert_int_equal (0, los_dev_ioctl (NULL, 10, NULL, 0));
    assert_int_equal (1, los_dev_ioctl (dev_t, 10, NULL, 0));
}

static void test_los_dev_seek (void **state)
{
    static los_dev_t dev_t;

    dev_t = los_dev_open ("dev2", O_RDWR);
    assert_int_not_equal (NULL, dev_t);

    assert_int_equal (-1, los_dev_seek (NULL, 0, 1));
}

static void test_los_dev_unregister (void **state)
{
    assert_int_equal (0, los_driv_unregister (NULL));
    assert_int_equal (0, los_driv_unregister ("dev7"));
}

/** test entry */

void test_dev (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_los_dev_register),
        unit_test (test_los_dev_open),
        unit_test (test_los_dev_close),
        unit_test (test_los_dev_read),
        unit_test (test_los_dev_write),
        unit_test (test_los_dev_ioctl),
        unit_test (test_los_dev_seek),
        unit_test (test_los_dev_unregister),
        };

    los_driv_init ();

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_dev", test_dev, setup, teardown, 0);