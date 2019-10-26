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

#ifndef __MEMTRY_H__
#define __MEMTRY_H__

#include <compiler.h>
#include <module.h>
#include <errno.h>

#include <arch/config.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#ifndef MEMTRY_MAX_ORDER
#define MEMTRY_MAX_ORDER            3
#endif

#define ERRNO_MEMTRY_ILLEGAL_ORDER  ERRNO_JOIN (MID_MEMTRY, 1)
#define ERRNO_MEMTRY_NO_ACCESS      ERRNO_JOIN (MID_MEMTRY, 2)

/* externs */

extern int mem_try (void * dst, void * src, int order);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MEMTRY_H__ */

