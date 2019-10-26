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

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <stdint.h>

#include <list.h>
#include <task.h>
#include <module.h>
#include <errno.h>
#include <tick.h>
#include <obj.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* typedefs */

typedef struct task * task_id;

typedef struct mutex
    {
    obj_t        obj;
    uint16_t     recurse;
    uint8_t      prio;              /* the max prio of the tasks pend on this mutex */
    task_id      owner;
    dlist_t      pend_q;
    dlist_t      node;              /* linked in task_t->owned_mutex */
    } mutex_t, * mutex_id;

/* defines */

#define ERRNO_MUTEX_ILLEGAL_ID          ERRNO_JOIN (MID_MUTEX, 1)
#define ERRNO_MUTEX_TIMEOUT             ERRNO_JOIN (MID_MUTEX, 2)
#define ERRNO_MUTEX_DELETED             ERRNO_JOIN (MID_MUTEX, 3)
#define ERRNO_MUTEX_UNAVAILABLE         ERRNO_JOIN (MID_MUTEX, 4)
#define ERRNO_MUTEX_ILLEGAL_OPERATION   ERRNO_JOIN (MID_MUTEX, 5)

/* externs */

extern int      mutex_init      (mutex_id);
extern mutex_id mutex_create    (void);
extern mutex_id mutex_open      (const char *, int, ...);
extern int      mutex_destroy   (mutex_id);
extern int      mutex_timedlock (mutex_id, unsigned int);
extern int      mutex_lock      (mutex_id);
extern int      mutex_trylock   (mutex_id);
extern int      mutex_unlock    (mutex_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MUTEX_H__ */

