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

#include <compiler.h>
#include <syscall.h>
#include <warn.h>

__weak const struct syscall_table syscall_table_task     = {0, NULL};
__weak const struct syscall_table syscall_table_mutex    = {0, NULL};
__weak const struct syscall_table syscall_table_sem      = {0, NULL};
__weak const struct syscall_table syscall_table_event    = {0, NULL};
__weak const struct syscall_table syscall_table_mq       = {0, NULL};
__weak const struct syscall_table syscall_table_heap     = {0, NULL};
__weak const struct syscall_table syscall_table_mempool  = {0, NULL};
__weak const struct syscall_table syscall_table_memtry   = {0, NULL};
__weak const struct syscall_table syscall_table_errno    = {0, NULL};
__weak const struct syscall_table syscall_table_kprintf  = {0, NULL};
__weak const struct syscall_table syscall_table_mpu      = {0, NULL};
__weak const struct syscall_table syscall_table_customer = {0, NULL};

/**
 * syscall_get_entry - errno set routine
 * @errno: the errno
 *
 * return: NA
 */

uintptr_t syscall_get_entry (syscall_id sid)
    {
    const struct syscall_table * entry;
    unsigned int                 rid;

    switch (SYSNUM_GET_MID (sid))
        {
        case MID_TASK:
            entry = &syscall_table_task;
            break;
        case MID_MUTEX:
            entry = &syscall_table_mutex;
            break;
        case MID_SEM:
            entry = &syscall_table_sem;
            break;
        case MID_EVENT:
            entry = &syscall_table_event;
            break;
        case MID_MQ:
            entry = &syscall_table_mq;
            break;
        case MID_HEAP:
            entry = &syscall_table_heap;
            break;
        case MID_MEMPOOL:
            entry = &syscall_table_mempool;
            break;
        case MID_MEMTRY:
            entry = &syscall_table_memtry;
            break;
        case MID_ERRNO:
            entry = &syscall_table_errno;
            break;
        case MID_KPRINTF:
            entry = &syscall_table_kprintf;
            break;
        case MID_CUSTOMER:
            entry = &syscall_table_customer;
            break;
        default:
            errno = ERRNO_SYSCALL_ILLEGAL_MID;
            return (uintptr_t) NULL;
        }

    rid = SYSNUM_GET_RID (sid);

    if (rid >= entry->nr_entries)
        {
        WARN ("Invalid syscall id!");
        errno = ERRNO_SYSCALL_ILLEGAL_RID;
        return (uintptr_t) NULL;
        }

    return entry->entries [rid];
    }

