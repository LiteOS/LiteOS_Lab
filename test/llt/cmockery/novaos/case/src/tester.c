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

#include <string.h>

#include <cmder.h>
#include <kprintf.h>

static tester_case_t *case_table;
static unsigned int case_num;

static void __tester_usage (void)
    {
    kprintf ("\n");
    kprintf ("Command: tester\n");
    kprintf ("   info: Execute test cases.\n");
    kprintf (" format: tester [-l | -a | testnames]\n");
    kprintf ("   usage:\n");
    kprintf ("       tester\n");
    kprintf ("           Show tester help information\n");
    kprintf ("       tester -l\n");
    kprintf ("           List all test cases.\n");
    kprintf ("       tester -a\n");
    kprintf ("           Run all test cases.\n");
    kprintf ("       tester test1 test2 ...\n");
    kprintf ("           Run 'test1' 'test2'\n");
    kprintf ("\n");
    }

static void __tester_statistics (void)
    {
    unsigned int i;
    unsigned int totals = 0, fails = 0;

    kprintf ("\nstatistics:\n");
    for (i = 0; i < case_num; ++i)
        {
        kprintf ("%-15s: totals %2d, fails %2d\n", case_table[i].name,
            case_table[i].stats->totals, case_table[i].stats->fails);
        totals += case_table[i].stats->totals;
        fails  += case_table[i].stats->fails;
        }
    kprintf ("\n  totals: %d, fails: %d\n\n", totals, fails);
    }

static void __tester_init (void)
    {
    case_table = (tester_case_t *) (__section_start__ (TESTER_SECTION) );
    case_num = (unsigned int) (__section_end__ (TESTER_SECTION) -
                               __section_start__ (TESTER_SECTION) );

    case_num /= sizeof (tester_case_t);
    }

static tester_case_t *__tester_match (const char *name)
    {
    unsigned int i;

    for (i = 0; i < case_num; ++i)
        {
        if (strcmp (case_table[i].name, name) == 0)
            {
            return &case_table[i];
            }
        }
    return NULL;
    }

static void __tester_list (void)
    {
    unsigned int i;

    if (case_num == 0)
        {
        kprintf ("nothing...\n");
        return;
        }

    kprintf ("\ntest cases:\n");
    for (i = 0; i < case_num; ++i)
        {
        kprintf ("   %s\n", case_table[i].name);
        }
    kprintf ("\n  totals %d\n\n", case_num);
    }

static int __tester_run (tester_case_t *caser)
    {
    if (caser->setup != NULL)
        {
        caser->setup ();
        }
    if (caser->entry != NULL)
        {
        caser->entry (caser);
        }
    if (caser->teardown != NULL)
        {
        caser->teardown ();
        }
    return 0;
    }

static int __tester_run_all (void)
    {
    unsigned int i;

    for (i = 0; i < case_num; ++i)
        {
        __tester_run (&case_table[i]);
        }
    return 0;
    }

static int __tester_run_some (int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++)
        {
        tester_case_t *caser = __tester_match (argv[i]);
        if (caser != NULL)
            {
            __tester_run (caser);
            }
        else
            {
            kprintf ("\"%s\" testcase not found!\n", argv[1]);
            }
        }
    return 0;
}

static int __cmd_tester (cmder_t *cmder, int argc, char *argv[])
    {
    __tester_init ();

    if (argc > 1)
        {
        if (strcmp (argv[1], "-l") == 0)
            {
            __tester_list ();
            }
        else if (strcmp (argv[1], "-a") == 0)
            {
            __tester_run_all ();
            __tester_statistics ();
            }
        else
            {
            __tester_run_some (argc, argv);
            }
        }
    else
        {
        __tester_usage ();
        }

    return 0;
    }

CMDER_CMD_DEF ("tester", "execute test cases, usage: 'tester [-l | -a | testnames]'", __cmd_tester);
