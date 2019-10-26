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

#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <stdint.h>
#include <stdbool.h>

#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

/* typedefs */

struct profile_data
    {
    task_id                  task;          /* NULL if system */
    bool                     tsc_started;
    unsigned long            tsc_start;
    /* timestamp accumulator */
    unsigned long long       tsc_all;
    };

/* externs */

extern void profile_sys_enter  (void);
extern void profile_sys_exit   (void);
extern void profile_task_enter (task_id);
extern int  profile_start      (void);
extern int  profile_foreach    (void (*) (struct profile_data *, uintptr_t), uintptr_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PROFILE_H__ */
