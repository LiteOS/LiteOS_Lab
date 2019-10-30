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

#include <kconfig.h>
#include <memtry.h>
#include <errno.h>
#include <warn.h>

#ifdef CONFIG_SYSCALL
#include <syscall.h>
#endif

#include <arch/interface.h>

/* externs */

/**
 * mem_try - try to copy from <src> to <dst> 1 (1 << <order>) bytes unit
 * @dst:   the address will try write to
 * @src:   the address will try read from
 * @order: try to copy (1 << order) bytes
 *
 * return: 0 on success, negtive value on error
 */

int mem_try (void * dst, void * src, int order)
    {
    WARN_ON (order > MEMTRY_MAX_ORDER,
             errno = ERRNO_MEMTRY_ILLEGAL_ORDER; return -1,
             "Invalid order!");

    if (mem_try_arch (dst, src, order) != 0)
        {
        errno = ERRNO_MEMTRY_NO_ACCESS;
        return -1;
        }

    return 0;
    }

#ifdef CONFIG_SYSCALL
static const uintptr_t syscall_entries_memtry [] =
    {
    (uintptr_t) mem_try,
    };

const struct syscall_table syscall_table_memtry =
    {
    ARRAY_SIZE (syscall_entries_memtry),
    syscall_entries_memtry
    };
#endif

