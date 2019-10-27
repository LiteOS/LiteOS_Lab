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

#include <string.h>
#include <stdlib.h>

#include <cmder.h>
#include <profile.h>
#include <task.h>

/* locals */

static unsigned long __tsc_freq = 0;

static void __top_print (struct profile_data * data, uintptr_t arg)
    {
    unsigned long long tsc_all = data->tsc_all;
    cmder_t          * cmder = (cmder_t *) arg;
    unsigned int       xx, yy;                  // xx.yy%, for example: 12.34%
    char             * name = data->task == NULL ? "system" : data->task->name;

    xx = (unsigned int) (tsc_all *   100 / __tsc_freq);
    yy = (unsigned int) (tsc_all * 10000 / __tsc_freq - xx * 100);

    cmder_printf (cmder, "%*s %02d.%02d%%\n", -(MAX_TASK_NAME_LEN - 1),
                  name, xx, yy);
    }

static __noreturn int __top_task (uintptr_t arg)
    {
    cmder_t * cmder = (cmder_t *) arg;

    while (1)
        {
        profile_start ();

        cmder_printf (cmder, "%c", 12);
        cmder_printf (cmder, "NAME        %%CPU\n");

        profile_foreach (__top_print, arg);
        }
    }

static int __cmd_top (cmder_t * cmder, int argc, char * argv [])
    {
    task_id top_task;

    __tsc_freq = tick_tsc_freq ();

    if (unlikely (__tsc_freq == 0))
        {
        cmder_printf (cmder, "error: fail to get system timestamp period!\n");
        return -1;
        }

    top_task = task_spawn ("top", 1, 0, 0x500, __top_task, (uintptr_t) cmder);

    if (unlikely (top_task == NULL))
        {
        cmder_printf (cmder, "error: fail to create top task!\n");
        return -1;
        }

    while (1)
        {
        unsigned char input = cmder->getc (cmder->arg);

        switch (input)
            {
            case 'q':
            case cmder_vk_ctrlc:
            case cmder_vk_ctrld:
                task_delete (top_task);
                return 0;
            default:
                break;
            }
        }
    }

CMDER_CMD_DEF ("top", "show task information", __cmd_top);
