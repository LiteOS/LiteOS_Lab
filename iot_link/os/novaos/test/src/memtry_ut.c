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

#include <timer.h>
#include <memtry.h>
#include <sem.h>
#include <bug.h>
#include <kprintf.h>
#include <cmder.h>

static timer_t tim1;

static sem_t sem;

static void __timer_once (uintptr_t arg)
    {
    uint8_t  u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

    (void) arg;

    if (mem_try (&u8,  (void *) 0x3fffffff, 0) != -1)
        {
        __bug ("memtry on 0x3fffffff, 2^0 bytes not return -1!");
        }

    if (mem_try (&u16, (void *) 0x3ffffffe, 1) != -1)
        {
        __bug ("memtry on 0x3ffffffe, 2^1 bytes not return -1!");
        }

    if (mem_try (&u32, (void *) 0x3ffffffc, 2) != -1)
        {
        __bug ("memtry on 0x3ffffff0, 2^2 bytes not return -1!");
        }

    if (mem_try (&u64, (void *) 0x3ffffff8, 3) != -1)
        {
        __bug ("memtry on 0x3fffffff, 2^3 bytes not return -1!");
        }

    sem_post (&sem);
    }

static int __cmd_memtry_test (cmder_t * cmder, int argc, char * argv [])
    {
    sem_init (&sem, 0);

    /* call the test case in task context */

    __timer_once (0);

    /* eat the post ... */

    sem_wait (&sem);

    /* call the test case in IRQ context */

    timer_init (&tim1, 0, 3, __timer_once, 0x1234);

    timer_start (&tim1);

    sem_wait (&sem);

    kprintf ("test pass!\n");

    return 0;
    }

CMDER_CMD_DEF ("memtry_test", "memtry test", __cmd_memtry_test);
