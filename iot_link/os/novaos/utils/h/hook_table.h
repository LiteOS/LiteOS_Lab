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

#ifndef __HOOK_TABLE_H__
#define __HOOK_TABLE_H__

#include <mutex.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define hook_table_foreach(idx, table, slots)               \
    for (idx = 0; idx < slots && table [idx] != NULL; idx++)

/* externs */

extern int hook_table_add (uintptr_t *, int, uintptr_t);
extern int hook_table_del (uintptr_t *, int, uintptr_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
