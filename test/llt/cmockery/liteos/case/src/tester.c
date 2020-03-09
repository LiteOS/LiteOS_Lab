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

#include <string.h>
#include <stdio.h>

#include <shell.h>

#define kprintf printf

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
        kprintf ("%-15s: totals %2lu, fails %2lu\n", case_table[i].name,
            case_table[i].stats->totals, case_table[i].stats->fails);
        totals += case_table[i].stats->totals;
        fails  += case_table[i].stats->fails;
        }
    kprintf ("\n  totals: %u, fails: %u\n\n", totals, fails);
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

static int __cmd_tester (int argc, char *argv[])
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

OSSHELL_EXPORT_CMD (__cmd_tester, "tester", "execute test cases, usage: 'tester [-l | -a | testnames]'");
