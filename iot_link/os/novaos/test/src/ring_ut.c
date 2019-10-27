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

#include <stdio.h>
#include <stdlib.h>

#include <ring.h>
#include <bug.h>
#include <cmder.h>

#include <task.h>

static uint64_t multi_byte_puts     = 0;
static uint64_t multi_byte_put_size = 0;
static uint64_t multi_byte_gets     = 0;
static uint64_t multi_byte_get_size = 0;
static uint64_t singl_byte_puts     = 0;
static uint64_t singl_byte_gets     = 0;

static __noreturn void __test_block (void)
    {
    ring_t        * ring = ring_create (63);
    unsigned char * pw   = malloc (64);
    unsigned char * pr   = malloc (64);

    int get_idx = 0;
    int put_idx = 0;

    if (ring != NULL)
        {
        __bug ("ring_create should return NULL if createing with size not 2^n aligned!");
        }

    ring = ring_create (64);

    if (ring == NULL)
        {
        __bug ("fail to create ring!");
        }

    for (;;)
        {
        unsigned int i = (unsigned int) rand ();
        unsigned int j = (unsigned int) rand () & 63;

        if (i & 1)
            {
            size_t expected = min (j, ring_len (ring));

            multi_byte_gets++;

            if (ring_get (ring, pr, j) != expected)
                {
                __bug ("number of bytes get from ring not expected!");
                }

            for (i = 0; i < expected; i++)
                {
                if (pr [i] != (get_idx++ & 0xff))
                    {
                    __bug ("content get from ring not expected!");
                    }
                }

            multi_byte_get_size += expected;
            }
        else
            {
            size_t expected;

            multi_byte_puts++;

            if (i & 2)
                {
                expected = min (j, 64 - ring_len (ring));

                for (i = 0; i < expected; i++)
                    {
                    pw [i] = put_idx & 0xff;
                    put_idx++;
                    }

                if (expected != ring_put (ring, pw, j))
                    {
                    __bug ("number of bytes put into ring not expected!");
                    }
                }
            else
                {
                expected = min (j, 64);

                for (i = 0; i < expected; i++)
                    {
                    pw [i] = put_idx & 0xff;
                    put_idx++;
                    }

                if (j > (64 - ring_len (ring)))
                    {
                    get_idx += (j - (64 - ring_len (ring)));
                    }

                if (expected != ring_put_force (ring, pw, j))
                    {
                    __bug ("number of bytes put into ring not expected!");
                    }
                }

            multi_byte_put_size += expected;
            }
        }
    }

static __noreturn void __test_byte (void)
    {
    ring_t * ring = ring_create (2);
    unsigned char r;

    int get_idx = 0;
    int put_idx = 0;

    ring = ring_create (2);

    for (;;)
        {
        unsigned int i = (unsigned int) rand ();

        if (i & 1)
            {
            size_t expected = ring_empty (ring) ? 0 : 1;

            if (ring_getc (ring, &r) != expected)
                {
                __bug ("number of bytes get from ring not expected!");
                }

            if (expected != 0)
                {
                if (r != (get_idx++ & 0xff))
                    {
                    __bug ("content get from ring not expected!");
                    }
                }

            singl_byte_gets++;
            }
        else
            {
            if (i & 2)
                {
                size_t expected = ring_full (ring) ? 0 : 1;

                if (expected != ring_putc (ring, (unsigned char) (put_idx & 0xff)))
                    {
                    __bug ("number of bytes put into ring not expected!");
                    }

                if (expected != 0)
                    {
                    put_idx++;
                    }
                }
            else
                {
                if (ring_full (ring))
                    {
                    get_idx++;
                    }

                if (1 != ring_putc_force (ring, (unsigned char) (put_idx & 0xff)))
                    {
                    __bug ("number of bytes put into ring not expected!");
                    }

                put_idx++;
                }

            singl_byte_puts++;
            }
        }
    }

task_id ring_test_task0;
task_id ring_test_task1;

static int __cmd_ring_test (cmder_t * cmder, int argc, char * argv [])
    {
    if (ring_test_task0 == NULL)
        {
        ring_test_task0 = task_spawn ("ring_t0", 20, 0, 0x500,
                                     (int (*) (uintptr_t)) __test_block, 0);
        ring_test_task1 = task_spawn ("ring_t1", 20, 0, 0x500,
                                     (int (*) (uintptr_t)) __test_byte, 0);
        }
    else
        {
        cmder_printf (cmder, "multi_byte_puts:     %lld\n", multi_byte_puts);
        cmder_printf (cmder, "multi_byte_put_size: %lld\n", multi_byte_put_size);
        cmder_printf (cmder, "multi_byte_gets:     %lld\n", multi_byte_gets);
        cmder_printf (cmder, "multi_byte_get_size: %lld\n", multi_byte_get_size);
        cmder_printf (cmder, "singl_byte_puts:     %lld\n", singl_byte_puts);
        cmder_printf (cmder, "singl_byte_gets:     %lld\n", singl_byte_gets);
        }

    return 0;
    }

CMDER_CMD_DEF ("ring_test", "ring test routine, the first invoking of the command create tasks, the left invoking will show running info", __cmd_ring_test);

