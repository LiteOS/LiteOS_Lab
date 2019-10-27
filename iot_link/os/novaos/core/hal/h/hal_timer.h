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

#include <stdbool.h>
#include <stdint.h>

#include <list.h>
#include <module.h>
#include <errno.h>

#ifndef __HAL_TIMER_H__
#define __HAL_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define HAL_TIMER_MAX_NAME_LEN              16

#define HAL_TIMER_MODE_ONE_SHOT             0
#define HAL_TIMER_MODE_REPEATED             1

#define ERRNO_HAL_TIMER_ILLEGAL_ID          ERRNO_JOIN (MID_HAL, 0x11)
#define ERRNO_HAL_TIMER_ILLEGAL_RANGE       ERRNO_JOIN (MID_HAL, 0x12)
#define ERRNO_HAL_TIMER_ILLEGAL_OPERATION   ERRNO_JOIN (MID_HAL, 0x13)
#define ERRNO_HAL_TIMER_ILLEGAL_NAME        ERRNO_JOIN (MID_HAL, 0x14)
#define ERRNO_HAL_TIMER_NOT_ENOUGH_TIMER    ERRNO_JOIN (MID_HAL, 0x15)

/* typedefs */

typedef struct hal_timer hal_timer_t;

typedef struct hal_timer_methods
    {
    int           (* enable)   (hal_timer_t *, unsigned long);
    int           (* disable)  (hal_timer_t *);
    void          (* postpone) (hal_timer_t *, unsigned long);
    void          (* prepone)  (hal_timer_t *, unsigned long);
    unsigned long (* counter)  (hal_timer_t *);
    unsigned long (* period)   (hal_timer_t *);
    } hal_timer_methods_t;

struct hal_timer
    {
    dlist_t          node;          /* node to be inserted in the timer list */
    const char     * name;
    uint8_t          mode;
    bool             busy;          /* timer allocated */
    unsigned long    freq;
    unsigned long    max_count;
    void             (* handler) (uintptr_t);
    uintptr_t        arg;

    const hal_timer_methods_t * methods;
    };

/* inlines */

/**
 * hal_timer_handler - timer isr handler invoked by driver
 * @timer: the timer
 *
 * return: NA
 */

static inline void hal_timer_handler (hal_timer_t * timer)
    {
    timer->handler (timer->arg);
    }

/* externs */

extern int           hal_timer_enable   (hal_timer_t * timer, uint8_t mode,
                                         unsigned long cmp_rld);
extern int           hal_timer_disable  (hal_timer_t * timer);
extern int           hal_timer_connect  (hal_timer_t * timer,
                                         void (* pfn) (uintptr_t),
                                         uintptr_t arg);
extern unsigned long hal_timer_counter  (hal_timer_t * timer);
extern int           hal_timer_postpone (hal_timer_t * timer, unsigned long count);
extern int           hal_timer_prepone  (hal_timer_t * timer, unsigned long count);
extern int           hal_timer_register (hal_timer_t * timer);
extern hal_timer_t * hal_timer_get      (const char * name);
extern unsigned long hal_timer_period   (hal_timer_t * timer);
extern int           hal_timer_feat_get (hal_timer_t * timer,
                                         uint32_t * max_count, uint32_t * freq);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HAL_TIMER_H__ */

