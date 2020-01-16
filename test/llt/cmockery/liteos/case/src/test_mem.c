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

#include "los_memory.h"
#include "heap.h"

#define TEST_MEM_SIZE   1024

static char *s_mem = NULL;

static void setup (void)
{
    assert_false (s_mem);
    assert_true (s_mem = LOS_MemAlloc (m_aucSysMem0, TEST_MEM_SIZE));
}

static void teardown (void)
{
    assert_int_equal (LOS_OK, LOS_MemFree (m_aucSysMem0, s_mem));
    s_mem = NULL;
}

static void test_LOS_MemInit (void **state)
{
    assert_int_equal (LOS_NOK, LOS_MemInit (NULL, 0));
    assert_int_equal (LOS_NOK, LOS_MemInit (s_mem, 0));

    assert_int_equal (LOS_OK, LOS_MemInit (s_mem, TEST_MEM_SIZE));
}

static void test_LOS_MemDeinit (void **state)
{
    printf ("warning: not implemented in os/liteos/base/mem/heap\r\n");
}

static void test_LOS_MemPoolList (void **state)
{
    printf ("warning: not implemented in os/liteos/base/mem/heap\r\n");
}

static void test_LOS_MemAlloc (void **state)
{
    UINT8 *pData = NULL;

    assert_false (pData = (UINT8 *)LOS_MemAlloc(NULL, 10));

    assert_true (pData = (UINT8 *)LOS_MemAlloc(s_mem, 10));
    assert_int_equal (LOS_OK, LOS_MemFree(s_mem, pData));

    assert_true (pData = (UINT8 *)LOS_MemAlloc(s_mem, 256));
    assert_int_equal (LOS_OK, LOS_MemFree(s_mem, pData));
}

static void test_LOS_MemAllocAlign (void **state)
{
    UINT8 *pData = NULL;

    assert_true (pData = (UINT8 *)LOS_MemAllocAlign(s_mem, 10, 1)); // automatic change
    assert_int_equal (LOS_OK, LOS_MemFree(s_mem, pData));

    assert_true (pData = (UINT8 *)LOS_MemAllocAlign(s_mem, 10, 4));
    assert_int_equal (LOS_OK, LOS_MemFree(s_mem, pData));

    assert_true (pData = (UINT8 *)LOS_MemAllocAlign(s_mem, 10, 8));
    assert_int_equal (LOS_OK, LOS_MemFree(s_mem, pData));
}

static void test_LOS_MemRealloc (void **state)
{
    UINT8 *pData = NULL;

    assert_true (pData = (UINT8 *)LOS_MemAlloc(s_mem, 10));
    assert_true (pData = (UINT8 *)LOS_MemRealloc(s_mem, pData, 8)); // cutdown
    assert_true (pData = (UINT8 *)LOS_MemRealloc(s_mem, pData, 10)); // expand
    assert_false (pData = (UINT8 *)LOS_MemRealloc(s_mem, pData, 0)); // free

    assert_true (pData = (UINT8 *)LOS_MemAllocAlign(s_mem, 10, 8));
    assert_true (pData = (UINT8 *)LOS_MemRealloc(s_mem, pData, 168)); // cutdown
    assert_false (pData = (UINT8 *)LOS_MemRealloc(s_mem, pData, 0)); // free

    assert_true (pData = (UINT8 *)LOS_MemAllocAlign(s_mem, 256, 8));
    assert_true (pData = (UINT8 *)LOS_MemRealloc(s_mem, pData, 168)); // cutdown
    assert_false (pData = (UINT8 *)LOS_MemRealloc(s_mem, pData, 0)); // free
    assert_true (pData = (UINT8 *)LOS_MemRealloc(s_mem, NULL, 20));
}

/** test entry */

void test_mem (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_LOS_MemInit),
        unit_test (test_LOS_MemDeinit),
        unit_test (test_LOS_MemPoolList),
        unit_test (test_LOS_MemAlloc),
        unit_test (test_LOS_MemAllocAlign),
        unit_test (test_LOS_MemRealloc),
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_mem", test_mem, setup, teardown, 0);