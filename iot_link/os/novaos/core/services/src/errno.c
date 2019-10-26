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

#include <common.h>
#include <task.h>
#include <irq.h>
#include <errno.h>
#include <bug.h>

#ifdef CONFIG_SYSCALL
#include <syscall.h>
#endif

/* locals */

static uint32_t __sys_error;

/**
 * errno_get - errno address get routine
 *
 * return: 0 on success, negtive value on error
 */

uint32_t * errno_get (void)
    {
    if (int_context () || current == NULL)
        {
        return &__sys_error;
        }
    else
        {
        return task_errno_get (current);
        }
    }

#ifdef CONFIG_SYSCALL
static const uintptr_t syscall_entries_errno [] =
    {
    (uintptr_t) errno_get,
    };

const struct syscall_table syscall_table_errno =
    {
    ARRAY_SIZE (syscall_entries_errno),
    syscall_entries_errno
    };
#endif

