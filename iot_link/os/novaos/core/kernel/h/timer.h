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

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

#include <tick.h>
#include <module.h>
#include <errno.h>
#include <obj.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TIMER_STAT_INACTIVE             0   /* not in tick_q */
#define TIMER_STAT_ACTIVE               1   /* in tick_q */
#define TIMER_STATUS_DELETING           3

#define TIMER_FLAG_ONE_SHOT             0
#define TIMER_FLAG_REPEATED             1

#define ERRNO_TIMER_ILLEGAL_ID          ERRNO_JOIN (MID_TIMER, 1)
#define ERRNO_TIMER_ILLEGAL_FLAG        ERRNO_JOIN (MID_TIMER, 2)
#define ERRNO_TIMER_ILLEGAL_INTERVAL    ERRNO_JOIN (MID_TIMER, 3)
#define ERRNO_TIMER_ILLEGAL_PFN         ERRNO_JOIN (MID_TIMER, 4)

/* typedefs */

typedef struct timer
    {
    obj_t              obj;
    uint16_t           status;
    uint16_t           flags;
    struct tick_q_node tq_node;
    unsigned long      interval;
    void            (* pfn) (uintptr_t);
    uintptr_t          arg;
    } timer_t, * timer_id;

/* externs */

extern int      timer_init   (timer_id, uint16_t, unsigned long,
                              void (*)(uintptr_t), uintptr_t);
extern timer_id timer_create (uint16_t, unsigned long, void (*) (uintptr_t),
                              uintptr_t);
extern int      timer_start  (timer_id);
extern int      timer_stop   (timer_id);
extern int      timer_delete (timer_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIMER_H__ */
