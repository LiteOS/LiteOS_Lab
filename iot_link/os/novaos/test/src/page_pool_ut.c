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

/*
 * note: this file can also be compiled on a x86/x64 host
 * for msys2, linux:
 *     gcc page_pool_ut.c -D__HOST__ -I../../include -I../../core/mem/h/ -I../h -I../../utils/h/ -o page_pool_ut
 */

#include <stdlib.h>

#include <page.h>

#ifdef __HOST__
#include <stdarg.h>
#include <stdio.h>

volatile int bug_dummy_check = 0;

void __bug (const char * str)
    {
    printf ("fatal error, bug: %s\n", str);
    while (!bug_dummy_check) {};
    }

#define kprintf printf

#include "../../core/mem/src/page.c"
#include "../../utils/src/rbtree.c"
#else
#include <cmder.h>
#include <kprintf.h>
#include <bug.h>
#endif

struct range
    {
    uint32_t pfn;
    uint32_t pages;
    };

static page_pool_t page_pool;

#define TEST_NODES              1024
#define TEST_NODES_MASK         (TEST_NODES - 1)

static uint32_t test_nodes [TEST_NODES] = {0};
static uint16_t test_pages [TEST_NODES] = {0};

static struct range ranges [] =
    {
        { 0x00000000, 0x1000 },
        { 0x00010000, 0xf000 },
        { 0x000f0000, 0x0100 },
        { 0x00000000, 0x0000 },
    };

static void __free_all (void)
    {
    int i;

    for (i = 0; i < TEST_NODES; i++)
        {
        if (test_pages [i] == 0)
            {
            continue;
            }

        page_pool_put (&page_pool, test_nodes [i], test_pages [i]);

        test_pages [i] = 0;
        }
    }

static int __verify_pool (void)
    {
    rb_node_t    * rbn = rb_first (&page_pool.pfn_tree);
    struct range * range;

    while (rbn != NULL)
        {
        page_node_t * node  = container_of (rbn, page_node_t, pfn_node.bin);
        bool          match = false;

        range = ranges;

        while (range->pages)
            {
            if ((node->pfn >= range->pfn) &&
                (node->pfn + node->pages <= range->pfn + range->pages))
                {
                match = true;
                break;
                }

            range++;
            }

        if (!match)
            {
            return -1;
            }

        rbn = rb_next (rbn);
        }

    return 0;
    }

static void __noreturn __stress_test (void)
    {
    static uint16_t sizes [32] =
        {
        4, 4, 4, 4, 4, 4, 4,                    /* 7 4s */
        8, 8, 8, 8, 8, 8,                       /* 6 8s */
        16, 16, 16, 16,                         /* 4  16s */
        32, 32, 32,
        64, 64, 64,
        128, 128,
        256, 256,
        512, 512,
        1024, 2048, 4096
        };

    int      idx;
    uint16_t pages;

#ifdef __HOST__
    uint64_t counter = 0;
#else
    int      counter = 0;
#endif

#ifndef __HOST__
    task_delay (2);
#endif

     while (1)
        {
        counter++;

        if (__verify_pool () == 0)
            {
#ifdef VERBOSE
            kprintf ("heap verify sucess\n");
#endif
            }
        else
            {
            __bug ("heap verify fail!");
            }

#ifdef __HOST__
#define PRINT_COUNT_SHIFT   24
#define PRINT_COUNT         (1 << PRINT_COUNT_SHIFT)
#define PRINT_COUNT_MASK    (PRINT_COUNT - 1)

        if (!(counter & PRINT_COUNT_MASK))
            {
            if (__verify_pool () != 0)
                {
                __bug ("heap verify fail!");
                }

            kprintf ("%lld x 0x1000000 times\n", counter >> PRINT_COUNT_SHIFT);
            fflush (stdout);
            }
#endif

        if (!(counter & 0xfffff))
            {
            __free_all ();

            continue;
            }

        idx = rand () & TEST_NODES_MASK;

        if (test_pages [idx] != 0)
            {
            page_pool_put (&page_pool, test_nodes [idx], test_pages [idx]);
            test_nodes [idx] = 0;
            test_pages [idx] = 0;
            continue;
            }

        pages  = sizes [rand () & 31];

        if (page_pool_get (&page_pool, pages, &test_nodes [idx]) == 0)
            {
            test_pages [idx] = pages;
            }
        }
    }

static void __dump_pool (void)
    {
    rb_node_t * rbn = rb_first (&page_pool.pfn_tree);

    kprintf ("dumping page pool:\n");
    kprintf ("pfn        - pages\n");

    while (rbn != NULL)
        {
        page_node_t * node = container_of (rbn, page_node_t, pfn_node.bin);

        kprintf ("0x%08x - 0x%08x\n", node->pfn, node->pages);

        rbn = rb_next (rbn);
        }
    }

#ifdef __HOST__
int main (void)
#else
static __noreturn int __ppt (void)
#endif
    {
    struct range * range = ranges;

    page_pool_init (&page_pool);

    while (range->pages)
        {
        page_pool_put (&page_pool, range->pfn, range->pages);
        __dump_pool ();

        range++;
        }

    __stress_test ();
    }

#ifndef __HOST__
static int __cmd_pagepool_test (cmder_t * cmder, int argc, char * argv [])
    {
    task_spawn ("ppt", 10, 0, 0x400, (int (*) (uintptr_t)) __ppt, 0);

    return 0;
    }

CMDER_CMD_DEF ("page_pool_test", "page pool test", __cmd_pagepool_test);
#endif

