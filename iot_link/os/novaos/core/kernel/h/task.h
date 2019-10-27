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

#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#include <config.h>

#include <list.h>
#include <config.h>
#include <compiler.h>
#include <defer.h>
#include <tick.h>
#include <module.h>
#include <errno.h>
#include <obj.h>

#include <arch/regset.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */

#define MAX_TASK_NAME_LEN               12

#define TASK_STATUS_READY               0u
#define TASK_STATUS_SUSPEND             1u
#define TASK_STATUS_PEND                2u
#define TASK_STATUS_DELAY               4u
#define TASK_STATUS_INVALID             UINT_MAX

#define TASK_OPTION_SYSTEM              0x00000001
#define TASK_OPTION_NO_BLOCK            0x00000002
#define TASK_OPTION_STATIC_TCB          0x00010000
#define TASK_OPTION_STATIC_STACK        0x00020000

#ifdef  CONFIG_TASK_PRIO_64
#define NR_TASK_PRIOS                   64
#else
#define NR_TASK_PRIOS                   32
#endif

#define TASK_PRIO_MAX                   (NR_TASK_PRIOS - 1)
#define TASK_PRIO_MIN                   (0)
#define TASK_PRIO_INV                   (NR_TASK_PRIOS)

#define ERRNO_TASK_ILLEGAL_OPERATION    ERRNO_JOIN (MID_TASK, 1)
#define ERRNO_TASK_ILLEGAL_ENTRY        ERRNO_JOIN (MID_TASK, 2)
#define ERRNO_TASK_ILLEGAL_NAME         ERRNO_JOIN (MID_TASK, 3)
#define ERRNO_TASK_ILLEGAL_PRIO         ERRNO_JOIN (MID_TASK, 4)
#define ERRNO_TASK_ILLEGAL_STACK_SIZE   ERRNO_JOIN (MID_TASK, 5)
#define ERRNO_TASK_ILLEGAL_TCB          ERRNO_JOIN (MID_TASK, 6)
#define ERRNO_TASK_ILLEGAL_TLS_INDEX    ERRNO_JOIN (MID_TASK, 7)
#define ERRNO_TASK_SUSPENDED            ERRNO_JOIN (MID_TASK, 8)
#define ERRNO_TASK_NOT_ENOUGH_SLOTS     ERRNO_JOIN (MID_TASK, 9)
#define ERRNO_TASK_ILLEGAL_STACK        ERRNO_JOIN (MID_TASK, 10)

/* used for stack overflow checking */

#define TASK_STACK_MAGIC                0x466348614974516aull

/* typedefs */

typedef struct mutex     * mutex_id;            /* forward declaretion */

typedef struct task
    {
    struct regset          regset;
    unsigned int           status;
    uint32_t               option;

    char                 * stack_base;
    size_t                 stack_size;

    /* priority values */

    uint8_t                c_prio;
    uint8_t                o_prio;

    unsigned int           tick_slices;

    int                 (* entry) (uintptr_t);
    uintptr_t              arg;

    union
        {
        dlist_t            rq_node;     /* ready queue node */
        struct tick_q_node tq_node;     /* tick queue node */
        };

    dlist_t                pq_node;

    /* ipc related feilds */

    dlist_t                mutex_owned;

    struct
        {
        obj_id             id;
        void             * pdata;
        } wanted;

    obj_t                  obj;

    union
        {
        char               name [MAX_TASK_NAME_LEN];
        deferred_job_t     job;
        };

    uint32_t               error;

#ifdef CONFIG_TASK_TLS

    /* task local storage, usually an integer or pointer */

    uintptr_t              tls [CONFIG_NR_TLS_SLOTS];

#endif
    } task_t, * task_id;

typedef int  (* task_create_pfn)  (task_id);
typedef void (* task_delete_pfn)  (task_id);
typedef void (* task_switch_pfn)  (task_id, task_id);
typedef int  (* task_foreach_pfn) (task_id, uintptr_t);


/* inlines */

static __always_inline void task_errno_set (task_id task, uint32_t error)
    {
    task->error = error;
    }

static __always_inline uint32_t * task_errno_get (task_id task)
    {
    return &task->error;
    }

/* externs */

extern task_id        current;
extern task_t         idle [];
extern unsigned int   task_lock_cnt;

extern void           task_idle_hook_set   (void (*) (void));
extern int            task_init            (task_id, char *, const char *, uint8_t,
                                            uint32_t, size_t,
                                            int (*) (uintptr_t), uintptr_t);
extern task_id        task_create          (const char *, uint8_t, uint32_t, size_t,
                                            int (*) (uintptr_t), uintptr_t);
extern int            task_delete          (task_id);
extern int            task_exit            (void);
extern int            task_resume          (task_id);
extern int            task_suspend         (task_id);
extern task_id        task_spawn           (const char *, uint8_t, uint32_t, size_t,
                                            int (*) (uintptr_t), uintptr_t);
extern int            task_delay           (unsigned int);
extern void           task_lock            (void);
extern void           task_unlock          (void);
extern int            task_foreach         (task_foreach_pfn, uintptr_t);
extern void           task_sched_start     (void);
extern void           task_ready_q_add     (task_id);
extern void           task_ready_q_ins     (task_id);
extern void           task_ready_q_del     (task_id);
extern void           task_fwait_q_add     (dlist_t *, unsigned, void (*) (task_id));
extern void           task_pwait_q_add     (dlist_t *, unsigned, void (*) (task_id));
extern void           task_pwait_q_adj     (dlist_t *, task_id);
extern bool           task_stack_overflow  (task_id);
extern unsigned int   task_status_get      (task_id);
extern uint8_t        task_prio_get        (task_id);
extern int            task_prio_set        (task_id, uint8_t);

#ifdef CONFIG_TASK_TLS
extern int            task_tls_slot_alloc  (void);
extern int            task_tls_set         (task_id, int, uintptr_t);
extern int            task_tls_get         (task_id, int, uintptr_t *);
#endif

#ifdef CONFIG_TASK_CREATE_HOOK
extern int            task_create_hook_add (task_create_pfn);
#endif
#ifdef CONFIG_TASK_DELETE_HOOK
extern int            task_delete_hook_add (task_delete_pfn);
#endif
#ifdef CONFIG_TASK_SWITCH_HOOK
extern int            task_switch_hook_add (task_switch_pfn);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TASK_H__ */

