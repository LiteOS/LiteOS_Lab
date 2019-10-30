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

#include <stdint.h>

#include <mutex.h>
#include <warn.h>

/**
 * hook_table_add - add a routine to the a hook table
 * @hooks: the hook table
 * @slots: the number of slots in the table
 * @pfn:   the interrupt hook callback
 *
 * return: 0 on success, negtive value on error
 */

int hook_table_add (uintptr_t * hooks, int slots, uintptr_t pfn)
    {
    int i;

    task_lock ();

    for (i = 0; i < slots; i++)
        {
        if (hooks [i] == 0)
            {
            hooks [i] = pfn;
            break;
            }
        }

    task_unlock ();

    WARN_ON (i == slots,
             return -1,
             "Not enough hook slot, hook_table = %p, pfn = %p!", hooks, pfn);

    return 0;
    }

/**
 * hook_table_del - delete a routine from the a hook table
 * @hooks: the hook table
 * @slots: the number of slots in the table
 * @pfn:   the interrupt hook callback
 *
 * return: 0 on success, negtive value on error
 */

int hook_table_del (uintptr_t * hooks, int slots, uintptr_t pfn)
    {
    int i;

    task_lock ();

    for (i = 0; i < slots; i++)
        {
        if (hooks [i] == pfn)
            {
            hooks [i] = 0;
            break;
            }
        }

    while (i < (slots - 1))
        {
        if (hooks [i + 1] == 0)
            {
            break;
            }

        hooks [i] = hooks [i + 1];
        hooks [i + 1] = 0;
        i++;
        }

    task_unlock ();

    return 0;
    }

