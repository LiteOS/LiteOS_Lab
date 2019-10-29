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

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <module.h>
#include <errno.h>
#include <module.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define SYSNUM_DEF(mid, rid)        (((mid & 255) << 8) | (rid & 255))
#define SYSNUM_GET_MID(sysnum)      (sysnum >> 8)
#define SYSNUM_GET_RID(sysnum)      (sysnum & 255)

#define ERRNO_SYSCALL_ILLEGAL_MID   ERRNO_JOIN (MID_SYSCALL, 1)
#define ERRNO_SYSCALL_ILLEGAL_RID   ERRNO_JOIN (MID_SYSCALL, 2)

/* typedefs */

typedef unsigned int syscall_id;

struct syscall_table
    {
    unsigned int       nr_entries;
    const uintptr_t  * entries;
    };

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SYSCALL_H__ */
