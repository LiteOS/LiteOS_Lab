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

#ifndef __TICK_H__
#define __TICK_H__

#include <stdint.h>
#include <limits.h>

#include <list.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define NO_WAIT                       (0)
#define WAIT_FOREVER                  (UINT_MAX)

/* typedefs */

struct tick_q_node
    {
    dlist_t      node;
    unsigned int ticks_left;
    void      (* pfn) (struct tick_q_node *, uintptr_t);
    uintptr_t    arg;
    dlist_t    * head;          /* for safe deletion */
    };

/* externs */

extern unsigned long tick_tsc         (void);
extern unsigned long tick_tsc_period  (void);
extern unsigned long tick_tsc_freq    (void);
extern void          tick_q_del       (struct tick_q_node *);
extern void          tick_q_add       (struct tick_q_node *, unsigned int,
                                       void (*) (struct tick_q_node *, uintptr_t),
                                       uintptr_t);
extern uint32_t      tick_count_get   (void);
extern uint64_t      tick_count_get64 (void);
extern void          tick_hook_set    (void (*) (void),
                                       void (*) (void));
extern int           tick_start       (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TICK_H__ */

