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

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <config.h>
#include <common.h>
#include <class.h>
#include <list.h>
#include <heap.h>
#include <defer.h>
#include <task.h>
#include <irq.h>
#include <critical.h>
#include <errno.h>
#include <bug.h>
#include <init.h>
#include <warn.h>

#ifdef CONFIG_TASK_HOOK
#include <hook_table.h>
#endif

#ifdef CONFIG_CMDER
#include <cmder.h>
#endif

#ifdef CONFIG_SYSCALL
#include <syscall.h>
#endif

#include <arch/interface.h>
#include <arch/sync.h>

/* typedefs */

/* priority bmap type used in this module */

#ifdef CONFIG_TASK_PRIO_64
typedef uint64_t        __prio_bmap_t;
#else
typedef uint32_t        __prio_bmap_t;
#endif

struct ready_q
    {
    struct task       * highest;
    __prio_bmap_t       bmap;
    dlist_t             heads [NR_TASK_PRIOS];
    };

/* globals */

struct ready_q          ready_q       = {0};
unsigned int            task_lock_cnt = 0;
task_id                 current       = NULL;

/* so idle can be used as an pointer */

task_t                  idle [1];

/*
 * classes are defined as an array with only 1 element, so we can use it as an
 * pointer
 */

class_t                 task_class [1];

/* locals */

static void          (* __pfn_idle_hook) (void) = NULL;

static uint64_t         __idle_stack [MIN_STACK_SIZE / sizeof (uint64_t)];

#ifdef CONFIG_TASK_TLS
static uint32_t         __tls_bitmap = 0;
#endif

/* task hook tables */

#ifdef CONFIG_TASK_CREATE_HOOK
static task_create_pfn  __task_create_hooks [CONFIG_NR_TASK_CREATE_HOOK_SLOTS] = {NULL};
#endif
#ifdef CONFIG_TASK_DELETE_HOOK
static task_delete_pfn  __task_delete_hooks [CONFIG_NR_TASK_DELETE_HOOK_SLOTS] = {NULL};
#endif
#ifdef CONFIG_TASK_SWITCH_HOOK
static task_switch_pfn  __task_switch_hooks [CONFIG_NR_TASK_SWITCH_HOOK_SLOTS] = {NULL};
#endif

/* externs */

/* inlines */

static __always_inline bool __task_lib_inited (void)
    {
    return ready_q.highest != NULL;
    }

#ifdef CONFIG_TASK_CREATE_HOOK
static __always_inline int __walk_create_hook (task_id task)
    {
    int idx;

    hook_table_foreach (idx, __task_create_hooks, CONFIG_NR_TASK_CREATE_HOOK_SLOTS)
        {
        if (unlikely (__task_create_hooks [idx] (task) != 0))
            {
            return -1;
            }
        }

    return 0;
    }

/**
 * task_create_hook_add - add a routine to the task create hook table
 *
 * return: 0 on success, negtive value on error
 */

int task_create_hook_add (task_create_pfn pfn)
    {
    int ret = -1;

    ret  = hook_table_add ((uintptr_t *) __task_create_hooks,
                           CONFIG_NR_TASK_CREATE_HOOK_SLOTS, (uintptr_t) pfn);

    if (unlikely (ret != 0))
        {
        return -1;
        }

    if (unlikely (task_foreach ((task_foreach_pfn) pfn, 0) == 0))
        {
        return 0;
        }

    (void) hook_table_del ((uintptr_t *) __task_create_hooks,
                            CONFIG_NR_TASK_CREATE_HOOK_SLOTS, (uintptr_t) pfn);

    return -1;
    }

/**
 * task_create_hook_del - delete a routine from the task create hook table
 *
 * return: 0 on success, negtive value on error
 */

int task_create_hook_del (task_create_pfn pfn)
    {
    return hook_table_del ((uintptr_t *) __task_create_hooks,
                           CONFIG_NR_TASK_CREATE_HOOK_SLOTS, (uintptr_t) pfn);
    }
#endif

#ifdef CONFIG_TASK_DELETE_HOOK
static __always_inline void __walk_delete_hook (task_id task)
    {
    int idx;

    hook_table_foreach (idx, __task_delete_hooks, CONFIG_NR_TASK_DELETE_HOOK_SLOTS)
        {
        __task_delete_hooks [idx] (task);
        }
    }

/**
 * task_switch_hook_add - add a routine to the task delete hook table
 *
 * return: 0 on success, negtive value on error
 */

int task_delete_hook_add (task_delete_pfn pfn)
    {
    return hook_table_add ((uintptr_t *) __task_delete_hooks,
                           CONFIG_NR_TASK_DELETE_HOOK_SLOTS, (uintptr_t) pfn);
    }

/**
 * task_delete_hook_del - delete a routine from the task delete hook table
 *
 * return: 0 on success, negtive value on error
 */

int task_delete_hook_del (task_delete_pfn pfn)
    {
    return hook_table_del ((uintptr_t *) __task_delete_hooks,
                           CONFIG_NR_TASK_DELETE_HOOK_SLOTS, (uintptr_t) pfn);
    }
#endif

#ifdef CONFIG_TASK_SWITCH_HOOK
static __always_inline void __walk_switch_hook (task_id old, task_id new)
    {
    int idx;

    hook_table_foreach (idx, __task_switch_hooks, CONFIG_NR_TASK_SWITCH_HOOK_SLOTS)
        {
        __task_switch_hooks [idx] (old, new);
        }
    }

/**
 * task_switch_hook - walk through the task switch hook table and invoke them
 *
 * return: NA
 *
 * should only be invoked in <schedule>
 */

void task_switch_hook (task_id old, task_id new)
    {
    int idx = 0;

    BUG_ON (task_stack_overflow (old), "task (%p:%s) stack overflow\n", old, old->name);

    do
        {
        if (__task_switch_hooks [idx] == NULL)
            {
            break;
            }

        __task_switch_hooks [idx] (old, new);
        } while (++idx < CONFIG_NR_TASK_SWITCH_HOOK_SLOTS);
    }

/**
 * task_switch_hook_add - add a routine to the task switch hook table
 *
 * return: 0 on success, negtive value on error
 */

int task_switch_hook_add (task_switch_pfn pfn)
    {
    return hook_table_add ((uintptr_t *) __task_switch_hooks,
                           CONFIG_NR_TASK_SWITCH_HOOK_SLOTS, (uintptr_t) pfn);
    }

/**
 * task_switch_hook_del - delete a routine from the task switch hook table
 *
 * return: 0 on success, negtive value on error
 */

int task_switch_hook_del (task_switch_pfn pfn)
    {
    return hook_table_del ((uintptr_t *) __task_switch_hooks,
                           CONFIG_NR_TASK_SWITCH_HOOK_SLOTS, (uintptr_t) pfn);
    }
#endif

/**
 * idle_entry - the idle task loop
 *
 * return: NA, never return
 */

static __noreturn int idle_entry (uintptr_t dummy)
    {
    (void) dummy;

    for (;;)
        {
        if (__pfn_idle_hook != NULL)
            {
            __pfn_idle_hook ();
            }
        }
    }

/**
 * task_idle_hook_set - set the idle task call back function
 *
 * return: NA
 */

void task_idle_hook_set (void (* hook) (void))
    {
    __pfn_idle_hook = hook;
    }

static void __ready_q_init (void)
    {
    int i;

    ready_q.highest = idle;

    for (i = 0; i < NR_TASK_PRIOS; i++)
        {
        dlist_init (&ready_q.heads [i]);
        }
    }

/**
 * task_sched_start - task schedule start
 *
 * return: NA
 */

void task_sched_start (void)
    {
    if (unlikely (tick_start () != 0))
        {
        return;
        }

    /* sched_start should never return */

    sched_start ();
    }

/**
 * task_init - initialize a task
 * @task:       the task tcb struct
 * @stack:      the stack prepared for the task
 * @name:       the name of the task being created
 * @prio:       the priority of the task
 * @options:    options
 * @stack_size: the stack size the task required
 * @entry:      the entry point of the task
 * @arg:        the argument of the task
 *
 * return: the created task handler or NULL of fail
 */

int task_init (task_id task, char * stack, const char * name, uint8_t prio,
               uint32_t options, size_t stack_size, int (* entry) (uintptr_t),
               uintptr_t arg)
    {
    WARN_ON (task == NULL,
             errno = ERRNO_TASK_ILLEGAL_TCB;   return -1,
             "Invalid task id");

    WARN_ON (stack == NULL,
             errno = ERRNO_TASK_ILLEGAL_STACK; return -1,
             "Invalid stack!");

    WARN_ON (!aligned_at (stack, STACK_ALIGN) || !aligned_at (stack_size, STACK_ALIGN),
             errno = ERRNO_TASK_ILLEGAL_STACK; return -1,
             "Invalid stack!");

    return obj_init (task_class, &task->obj, stack, name, prio,
                     options | TASK_OPTION_STATIC_TCB | TASK_OPTION_STATIC_STACK,
                     stack_size, entry, arg);
    }

/**
 * task_create - create a task
 * @name:       the name of the task being created
 * @prio:       the priority of the task
 * @options:    options
 * @stack_size: the stack size the task required
 * @entry:      the entry point of the task
 * @arg:        the argument of the task
 *
 * return: the created task handler or NULL of fail
 */

task_id task_create (const char * name, uint8_t prio, uint32_t options,
                     size_t stack_size, int (* entry) (uintptr_t),
                     uintptr_t arg)
    {
    task_id task;
    char  * stack;

    /* add one extra uint64_t for task stack overflow checking */

    stack_size = stack_size_get (stack_size + sizeof (uint64_t));

    task = malloc (sizeof (task_t));

    if (unlikely (task == NULL))
        {
        return NULL;
        }

    stack = heap_alloc_align (kernel_heap, STACK_ALIGN, stack_size);

    if (unlikely (stack == NULL))
        {
        free (task);
        return NULL;
        }

    if (likely (obj_init (task_class, &task->obj, stack, name, prio, options,
                          stack_size, entry, arg) == 0))
        {
        return task;
        }

    free (stack);
    free (task);

    return NULL;
    }

/**
 * task_stack_overflow - check if a task's stack is overflow
 * @task: the task id
 *
 * return: true if overflow, false if not
 */

bool task_stack_overflow (task_id task)
    {
    if (unlikely (task == NULL))
        {
        return false;
        }

    return *((uint64_t *) task->stack_base) != TASK_STACK_MAGIC;
    }

static inline int __verify_context (task_id task)
    {
    WARN_ON (!__task_lib_inited (),
             errno = ERRNO_TASK_ILLEGAL_OPERATION; return -1,
             "Task lib not initialized!");

    WARN_ON (task == NULL,
             errno = ERRNO_TASK_ILLEGAL_TCB; return -1,
             "Invalid task id");

    return 0;
    }

/**
 * task_delete - delete a task
 * @task: the task to delete
 *
 * return: 0 for success, negtive value if fail
 */

int task_delete (task_id task)
    {
    if (unlikely (__verify_context (task) != 0))
        {
        return -1;
        }

    WARN_ON ((task->option & TASK_OPTION_SYSTEM) != 0,
             errno = ERRNO_TASK_ILLEGAL_OPERATION; return -1,
             "Can not delete system task!");

    if (task != current)
        {

        /*
         * task_lock and then verify the task to prevent the task exit or deleted
         * before the task_resume and the operations followed and prevent the
         * task preempted current task
         */

        task_lock ();

        if (unlikely (obj_verify (task_class, &task->obj) != 0))
            {
            task_unlock ();
            return -1;
            }

        task_resume (task);

        __regset_init    (&task->regset, task->stack_base + task->stack_size);
        __regset_set_pc  (&task->regset, (uintptr_t) task_exit);

        task_unlock ();

        return 0;
        }

    /*
     * unlock all the lock owned by current, dlist_foreach or dlist_foreach_safe
     * is not used here because mutex->recurse maybe greater than 1
     */

    while (!dlist_empty (&current->mutex_owned))
        {
        mutex_unlock (container_of (current->mutex_owned.next, mutex_t, node));
        }

    /*
     * in fact, this will not return, __task_destroy will remove current from
     * the ready_q
     */

    return obj_destroy (task_class, &task->obj);
    }

/**
 * task_exit - current task exit
 *
 * return: 0 for success, negtive value if fail
 */

int __noreturn task_exit (void)
    {
    task_delete (current);

    BUG ("We should never be here!");

    while (1)
        {
        /* guard, we should never be here */
        }
    }

/**
 * __task_resume - resume a task in critical region
 */

static int __task_resume (uintptr_t arg1, uintptr_t arg2)
    {
    task_id task = (task_id) arg1;

    (void) arg2;

    if (task->status == TASK_STATUS_READY)
        {
        return 0;
        }

    /* resuming a task pending on an IPC object, make it fail */

    if ((task->status & TASK_STATUS_PEND) != 0)
        {
        __regset_set_rv (&task->regset, -1);
        }

    if (task != idle)
        {
        task_ready_q_add (task);
        }

    return 0;
    }

/**
 * task_resume - resume a task
 * @task: the task to resume
 *
 * return: 0 for success, negtive value if fail
 */

int task_resume (task_id task)
    {
    int ret;

    if (unlikely (__verify_context (task) != 0))
        {
        return -1;
        }

    if (unlikely (obj_verify_protect (task_class, &task->obj) != 0))
        {
        return -1;
        }

    ret = do_critical (__task_resume, (uintptr_t) task, 0);

    obj_unprotect (&task->obj);

    return ret;
    }

static int __task_suspend (uintptr_t arg1, uintptr_t arg2)
    {
    task_id task = (task_id) arg1;

    (void) arg2;

    /* delete current task from any queue it is queued */

    if (likely (task->status == TASK_STATUS_READY))
        {
        task_ready_q_del (task);
        }
    else if (unlikely (task->status & (TASK_STATUS_DELAY | TASK_STATUS_PEND)))
        {
        __regset_set_rv (&task->regset, -1);

        task_errno_set (task, ERRNO_TASK_SUSPENDED);

        if (unlikely (task->status & TASK_STATUS_DELAY))
            {
            tick_q_del (&task->tq_node);
            }

        if (unlikely (task->status & TASK_STATUS_PEND))
            {
            dlist_del (&task->pq_node);
            }
        }

    task->status |= TASK_STATUS_SUSPEND;

    return 0;
    }

/**
 * task_suspend - suspend a task
 * @task: the task to suspend
 *
 * return: 0 for success, negtive value if fail
 */

int task_suspend (task_id task)
    {
    int ret;

    if (unlikely (__verify_context (task) != 0))
        {
        return -1;
        }

    WARN_ON (task->option & TASK_OPTION_NO_BLOCK,
             errno = ERRNO_TASK_ILLEGAL_OPERATION; return -1,
             "Trying to suspend noblock task, task name: \"%s\"!", task->name);

    if (unlikely (obj_verify_protect (task_class, &task->obj) != 0))
        {
        return -1;
        }

    ret = do_critical (__task_suspend, (uintptr_t) task, 0);

    obj_unprotect (&task->obj);

    return ret;
    }

/**
 * task_spawn - create a task and then resume it
 * @name:       the name of the task being created
 * @prio:       the priority of the task
 * @options:    options
 * @stack_size: the stack size the task required
 * @entry:      the entry point of the task
 * @arg:        the argument of the task
 *
 * return: the created task handler or NULL of fail
 */

task_id task_spawn (const char * name, uint8_t prio, uint32_t options,
                    size_t stack_size, int (* entry) (uintptr_t), uintptr_t arg)
    {
    task_id tid = task_create (name, prio, options, stack_size, entry, arg);

    if (likely (tid != NULL))
        {
        if (unlikely (task_resume (tid) != 0))
            {
            task_delete (tid);

            tid = NULL;
            }
        }

    return tid;
    }

/**
 * task_status_get - get the status of a task
 * @task: the given task if NULL current will be selected
 *
 * return: the status of the given task
 */

unsigned int task_status_get (task_id task)
    {
    unsigned int status;

    if (unlikely (obj_verify_protect (task_class, &task->obj) != 0))
        {
        return TASK_STATUS_INVALID;
        }

    status = task->status;

    obj_unprotect (&task->obj);

    return status;
    }

/**
 * task_prio_get - get the priority of a task
 * @task: the given task if NULL current will be selected
 *
 * return: the priority of the given task
 */

uint8_t task_prio_get (task_id task)
    {
    uint8_t prio;

    if (unlikely (obj_verify_protect (task_class, &task->obj) != 0))
        {
        return TASK_PRIO_INV;
        }

    prio = task->c_prio;

    obj_unprotect (&task->obj);

    return prio;
    }

static int __task_prio_set (uintptr_t arg1, uintptr_t arg2)
    {
    task_id   task = (task_id) arg1;
    uint8_t   prio = (uint8_t) arg2;

    if (prio == task->o_prio)
        {
        return 0;
        }

    if (task->wanted.id != NULL && task->wanted.id->class->mid == MID_MUTEX)
        {
        errno = ERRNO_TASK_ILLEGAL_OPERATION;
        return -1;
        }

    task->o_prio = prio;

    if (!dlist_empty (&task->mutex_owned) && prio > task->c_prio)
        {
        uint8_t   lowest_mutex_prio = TASK_PRIO_MAX;
        dlist_t * itr;

        /*
         * `task->c_prio < prio` and holding some mutexes, check if prio smaller
         * than all mutexes the task held
         */

        dlist_foreach (itr, &task->mutex_owned)
            {
            mutex_id mutex = container_of (itr, mutex_t, node);

            if (lowest_mutex_prio > mutex->prio)
                {
                lowest_mutex_prio = mutex->prio;
                }
            }

        prio = (prio <= lowest_mutex_prio) ? prio : lowest_mutex_prio;
        }

    if (task->c_prio == prio)
        {
        return 0;
        }

    if (task->status == TASK_STATUS_READY)
        {
        task_ready_q_del (task);
        task->c_prio = prio;
        task_ready_q_add (task);
        }
    else
        {
        task->c_prio = prio;
        }

    return 0;
    }

/**
 * task_prio_set - set the priority of a task
 * @task: the given task if NULL current will be selected
 * @prio: the new priority
 *
 * return: 0 on success, negtive value on fail when the prio is not valid or the
 *         task is pending on a mutex
 */

int task_prio_set (task_id task, uint8_t prio)
    {
    int ret;

    WARN_ON (task == NULL,
             errno = ERRNO_TASK_ILLEGAL_TCB;  return -1,
             "Invalid task id");

    WARN_ON (prio > TASK_PRIO_MAX,
             errno = ERRNO_TASK_ILLEGAL_PRIO; return -1,
             "Invalid priority!");

    if (unlikely (obj_verify_protect (task_class, &task->obj) != 0))
        {
        return -1;
        }

    ret = do_critical (__task_prio_set, (uintptr_t) task, (uintptr_t) prio);

    obj_unprotect (&task->obj);

    return ret;
    }

static void __task_tick_q_callback (struct tick_q_node * node, uintptr_t arg)
    {
    task_id task = container_of (node, struct task, tq_node);

    /*
     * needless verify the task, if the task is being deleted, it is firstly
     * resumed (deleted from any tick list and pend list) and then destroyed
     */

    if ((task->status & TASK_STATUS_PEND) != 0)
        {
        __regset_set_rv (&task->regset, -1);
        }

    /* now task now in tick queue */

    task->status &= ~TASK_STATUS_DELAY;

    task_ready_q_add (task);

    if (arg != 0)
        {
        ((void (*) (task_id)) arg) (task);
        }
    }

static int __task_delay (uintptr_t arg1, uintptr_t arg2)
    {
    unsigned int ticks = (unsigned int) arg1;

    (void) arg2;

    /* current task needless verify */

    task_ready_q_del (current);

    if (unlikely (ticks == 0))
        {
        task_ready_q_add (current);
        return 0;
        }

    current->status |= TASK_STATUS_DELAY;

    tick_q_add (&current->tq_node, ticks, __task_tick_q_callback, 0);

    return 0;
    }

/**
 * task_delay - delay a task from executing
 * @ticks: number of ticks to delay task
 *
 * return: 0, or negtive value if error
 */

int task_delay (unsigned int ticks)
    {
    WARN_ON (current == NULL,
             errno = ERRNO_TASK_ILLEGAL_OPERATION; return -1,
             "Invalid task id!");

    return do_critical_might_sleep (__task_delay, (uintptr_t) ticks, 0);
    }

/**
 * task_lock - disable the task preemptive
 *
 * return: NA
 */

void task_lock (void)
    {

    /* it is meaningless lock the schedule pre-kernel or in IRQ context */

    if (unlikely (current == NULL || int_context ()))
        {
        return;
        }

    task_lock_cnt++;
    wmb ();
    }

/**
 * task_unlock - enable the task preemptive
 *
 * return: NA
 */

void task_unlock (void)
    {
    if (unlikely (current == NULL || int_context () || task_lock_cnt == 0))
        {
        return;
        }

    task_lock_cnt--;

    if (unlikely (!in_critical () && !int_context ()))
        {
        (void) schedule (0);
        }
    }

static int __task_foreach (obj_id obj, va_list valist)
    {
    task_foreach_pfn pfn = va_arg (valist, task_foreach_pfn);
    uintptr_t        arg = va_arg (valist, uintptr_t);

    return pfn (container_of (obj, task_t, obj), arg);
    }

/**
 * task_foreach - invoke a callback function for each task
 * @pfn: the call back
 * @arg: the pravite argument
 *
 * return: 0, or negtive value if error
 */

int task_foreach (task_foreach_pfn pfn, uintptr_t arg)
    {
    return obj_foreach (task_class, __task_foreach, pfn, arg) == NULL ? 0 : -1;
    }

static inline void __ready_q_put (struct task * task, bool head)
    {
    uint8_t prio = task->c_prio;

    if (unlikely (task->status & TASK_STATUS_PEND))
        {
        dlist_del (&task->pq_node);
        task->status &= ~TASK_STATUS_PEND;
        }

    if (unlikely (task->status & TASK_STATUS_DELAY))
        {
        tick_q_del (&task->tq_node);
        task->status &= ~TASK_STATUS_DELAY;
        }

    task->status = TASK_STATUS_READY;

    if ((ready_q.highest == idle) || (prio < ready_q.highest->c_prio))
        {
        ready_q.highest = task;
        }

    ready_q.bmap |= ((__prio_bmap_t) 1) << (TASK_PRIO_MAX - prio);

    if (unlikely (head))
        {
        dlist_add (&ready_q.heads [prio], &task->rq_node);
        }
    else
        {
        task->tick_slices = 0;
        dlist_add_tail (&ready_q.heads [prio], &task->rq_node);
        }
    }

/**
 * task_ready_q_add - add a task to the tail of the ready queue with its priority
 * @task: the task to add
 *
 * invoked by kernel in critical only
 *
 * return: NA
 */

void task_ready_q_add (struct task * task)
    {
    BUG_ON (!in_critical (), "Invalid operation!");

    __ready_q_put (task, false);
    }

/**
 * task_ready_q_ins - add a task to the head of the ready queue with its priority
 * @task: the task to add
 *
 * return: NA
 */

void task_ready_q_ins (struct task * task)
    {
    __ready_q_put (task, true);
    }

/**
 * task_ready_q_del - remove a task from ready queue
 * @task: the task to remove
 *
 * return: NA
 */

void task_ready_q_del (struct task * task)
    {
    int     idx;
    uint8_t prio = task->c_prio;

    BUG_ON (!in_critical (), "Invalid operation!");

    dlist_del (&task->rq_node);

    if (dlist_empty (&ready_q.heads [prio]))
        {
        ready_q.bmap &= ~(((__prio_bmap_t) 1) << (TASK_PRIO_MAX - task->c_prio));
        }

    if (ready_q.highest != task)
        {
        return;
        }

    if (ready_q.bmap == 0)
        {
        ready_q.highest = idle;
        return;
        }

#ifdef CONFIG_TASK_PRIO_64
    idx = __clz_u64 (ready_q.bmap);
#else
    idx = __clz_u32 (ready_q.bmap);
#endif

    ready_q.highest = container_of (ready_q.heads [idx].next, struct task, rq_node);

    return;
    }

static inline void __task_q_xwait_timed (unsigned int timeout,
                                         void (* callback) (task_id task))
    {
    task_ready_q_del (current);

    current->status |= TASK_STATUS_PEND;

    if (timeout != WAIT_FOREVER)
        {
        current->status |= TASK_STATUS_DELAY;
        tick_q_add (&current->tq_node, timeout, __task_tick_q_callback,
                    (uintptr_t) callback);
        }
    }

/**
 * task_fwait_q_add - add current task to a fifo wait queue
 * @q:        the fifo wait queue
 * @timeout:  the max ticks the task wait in the queue
 * @callback: the timeout call back
 *
 * return: NA
 */

void task_fwait_q_add (dlist_t * q, unsigned int timeout,
                       void (* callback) (task_id task))
    {
    BUG_ON (!in_critical (), "Invalid operation!");

    dlist_add_tail (q, &current->pq_node);

    __task_q_xwait_timed (timeout, callback);
    }

static void __pwait_q_add (dlist_t * q, task_id task)
    {
    dlist_t * itr;

    dlist_foreach (itr, q)
        {
        if (task->c_prio < container_of (itr, task_t, pq_node)->c_prio)
            {
            break;
            }
        }

    /*
     * 1) if q is empty, itr will just be itr
     * 2) if all tasks in have higher priority than current, itr will be q
     * 3) otherwise, itr will be the first node that priority lower than current
     *
     * for all of these three conditions, just add current in front of q
     */

    dlist_add_tail (itr, &task->pq_node);
    }

/**
 * task_fwait_q_add - add current task to a priority wait queue
 * @q:        the priority wait queue
 * @timeout:  the max ticks the task wait in the queue
 * @callback: the timeout call back
 *
 * return: NA
 */

void task_pwait_q_add (dlist_t * q, unsigned int timeout,
                       void (* callback) (task_id task))
    {
    BUG_ON (!in_critical (), "Invalid operation!");

    __pwait_q_add (q, current);

    __task_q_xwait_timed (timeout, callback);
    }

/**
 * task_pwait_q_adj - correct the location in a priority queue when the priority
 *                    of a task changed
 * @q:    the priority wait queue
 * @task: the task whose priority changed and need correct loction
 *
 * return: NA
 */

void task_pwait_q_adj (dlist_t * q, task_id task)
    {
    BUG_ON (!in_critical (), "Invalid operation!");

    dlist_del (&task->pq_node);

    __pwait_q_add (q, task);
    }

#ifdef CONFIG_TASK_TLS

/**
 * task_tls_slot_alloc - allocate a TLS slot in TCB
 *
 * return: a positive index, or -1 if error
 */

int task_tls_slot_alloc (void)
    {
    int idx;

    if (__tls_bitmap == 0)
        {
        idx = 0;
        }
    else
        {
        idx = CONFIG_NR_TLS_SLOTS - __clz_u32 (__tls_bitmap);

        if (idx >= CONFIG_NR_TLS_SLOTS)
            {
            errno = ERRNO_TASK_NOT_ENOUGH_SLOTS;
            return -1;
            }
        }

    __tls_bitmap |= 1u << (32 - CONFIG_NR_TLS_SLOTS + idx);

    return idx;
    }

/**
 * task_tls_set - set tls value by index
 * @task:  the task TCB set to
 * @slot:  the TLS slot index
 * @value: the new value
 *
 * return: 0, or -1 if error
 */

int task_tls_set (task_id task, int slot, uintptr_t value)
    {
    WARN_ON (task == NULL,
             errno = ERRNO_TASK_ILLEGAL_TCB;  return -1,
             "Invalid task id");

    WARN_ON (slot < 0 || slot >= CONFIG_NR_TLS_SLOTS,
             errno = ERRNO_TASK_ILLEGAL_TLS_INDEX; return -1,
             "Invalid slot!");

    task->tls [slot] = value;

    return 0;
    }

/**
 * task_tls_get - get tls value by index
 * @task:  the task TCB get from
 * @slot:  the TLS slot index
 * @value: address to save the value
 *
 * return: 0, or -1 if error
 */

int task_tls_get (task_id task, int slot, uintptr_t * value)
    {
    WARN_ON (task == NULL,
             errno = ERRNO_TASK_ILLEGAL_TCB;  return -1,
             "Invalid task id");

    WARN_ON (slot < 0 || slot >= CONFIG_NR_TLS_SLOTS,
             errno = ERRNO_TASK_ILLEGAL_TLS_INDEX; return -1,
             "Invalid slot!");

    *value = task->tls [slot];

    return 0;
    }

#endif

static int __task_init (obj_id obj, va_list valist)
    {
    task_id task = container_of (obj, task_t, obj);

    char       * stack;
    const char * name;
    unsigned int prio;
    uint32_t     options;
    size_t       stack_size;
    int       (* entry) (uintptr_t);
    uintptr_t    arg;

    stack      = va_arg (valist, char *);
    name       = va_arg (valist, const char *);
    prio       = va_arg (valist, unsigned int);
    options    = va_arg (valist, uint32_t);
    stack_size = va_arg (valist, size_t);
    entry      = va_arg (valist, int (*) (uintptr_t));
    arg        = va_arg (valist, uintptr_t);

    WARN_ON (unlikely (prio > TASK_PRIO_MAX),
             errno = ERRNO_TASK_ILLEGAL_PRIO; return -1,
             "Invalid priority!");

    /* priority 0 is reserved for the deferred task */

    WARN_ON (prio == 0 && current != NULL && strcmp (name, CONFIG_DEFERRED_NAME) != 0,
             errno = ERRNO_TASK_ILLEGAL_PRIO; return -1,
             "Priority 0 is reserved for deferred task!");

    memset (stack, 0xee, stack_size);

    task->status           = TASK_STATUS_SUSPEND;

    task->option           = options;

    /* init priority feilds */

    task->c_prio           = (uint8_t) prio;
    task->o_prio           = (uint8_t) prio;

    task->tick_slices      = 0;

    task->entry            = entry;
    task->arg              = arg;

    task->stack_base       = stack;
    task->stack_size       = stack_size;

    *((uint64_t *) task->stack_base) = TASK_STACK_MAGIC;

#ifdef CONFIG_TASK_TLS
    memset (task->tls, 0, sizeof (task->tls));
#endif

    strncpy (task->name, name, MAX_TASK_NAME_LEN);

    task->name [MAX_TASK_NAME_LEN - 1] = '\0';

    dlist_init (&task->mutex_owned);

    task->wanted.id       = NULL;

    __regset_init    (&task->regset, stack + stack_size);
    __regset_set_pc  (&task->regset, (uintptr_t) entry);
    __regset_set_arg (&task->regset, 0, (uintptr_t) arg);
    __regset_set_lr  (&task->regset, (uintptr_t) task_exit);

#ifdef CONFIG_TASK_CREATE_HOOK
    if (unlikely (__walk_create_hook (task) != 0))
        {
        return -1;
        }
#endif

    /* set obj->name */

    obj->name = task->name;

    return 0;
    }

static void __task_delete_clean (uintptr_t arg)
    {
    task_id task = (task_id) arg;

    if ((task->option & TASK_OPTION_STATIC_STACK) == 0)
        {
        free (task->stack_base);
        }

    if ((task->option & TASK_OPTION_STATIC_TCB) == 0)
        {
        free (task);
        }
    }

static int __task_destroy_critical (uintptr_t arg1, uintptr_t arg2)
    {
    task_id task = (task_id) arg1;

    (void) arg2;

    /* delete current task from ready queue */

    task_ready_q_del (current);

    deferred_job_init (&task->job, __task_delete_clean, (uintptr_t) task);

    deferred_job_sched (&task->job);

    return 0;
    }

static int __task_destroy (obj_id obj)
    {

#ifdef CONFIG_TASK_DELETE_HOOK
    __walk_delete_hook (container_of (obj, task_t, obj));
#endif

    /*
     * this routine is always invoked under task context, and will never return
     * and there is no resources need to be freed in obj_destroy, that will be
     * done in deferred job
     */

    return do_critical (__task_destroy_critical,
                        (uintptr_t) container_of (obj, task_t, obj), 0);
    }

/**
 * task_lib_init - task library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int task_lib_init (void)
    {
    BUG_ON (class_init (task_class, MID_TASK, sizeof (task_t),
                        __task_init, __task_destroy, NULL, NULL),
            "fail to initialize task_class!");

    BUG_ON (task_init (idle, (char *) __idle_stack, "idle", 0,
                       TASK_OPTION_NO_BLOCK | TASK_OPTION_SYSTEM, MIN_STACK_SIZE,
                       idle_entry, 0),
            "fail to initialize idle task!");

    __ready_q_init ();

    idle->status = TASK_STATUS_READY;

    return 0;
    }

MODULE_INIT (kernel, task_lib_init);

#ifdef CONFIG_SYSCALL
static const uintptr_t syscall_entries_task [] =
    {
    (uintptr_t) task_create,
    (uintptr_t) task_delete,
    (uintptr_t) task_exit,
    (uintptr_t) task_resume,
    (uintptr_t) task_suspend,
    (uintptr_t) task_spawn,
    (uintptr_t) task_delay,
    (uintptr_t) task_lock,
    (uintptr_t) task_unlock,
    (uintptr_t) task_status_get,
    (uintptr_t) task_prio_get,
    (uintptr_t) task_prio_set,
    };

/* must be global to override weak symbol */

const struct syscall_table syscall_table_task =
    {
    ARRAY_SIZE (syscall_entries_task),
    syscall_entries_task
    };
#endif

#ifdef CONFIG_CMDER
static inline uint32_t * __task_stack_lowest (task_id task)
    {
    uint32_t * p = (uint32_t *) task->stack_base + sizeof (uint64_t);

    while (*p++ == 0xeeeeeeee);

    return p;
    }

static int __task_show (task_id task, uintptr_t arg)
    {
    char     * status;
    cmder_t  * cmder = (cmder_t *) arg;
    uint32_t * sp_lowest = __task_stack_lowest (task);
    uint32_t * sp_now    = (uint32_t *) __regset_get_sp (&task->regset);

    cmder_printf (cmder, "%*s", -(MAX_TASK_NAME_LEN - 1), task->name);
    cmder_printf (cmder, " %08x", (uintptr_t) task->entry);
    cmder_printf (cmder, " %08x", (uintptr_t) task);

    if (task == idle)
        {
        cmder_printf (cmder, "%6s", "NA");
        cmder_printf (cmder, "%6s", "NA");
        }
    else
        {
        cmder_printf (cmder, "%6d", task->o_prio);
        cmder_printf (cmder, "%6d", task->c_prio);
        }

    cmder_printf (cmder, " %08x", task->stack_base);
    cmder_printf (cmder, " %08x", sp_lowest <= sp_now ? sp_lowest : sp_now);
    cmder_printf (cmder, " %08x", sp_now);
    cmder_printf (cmder, " %08x", task->stack_base + task->stack_size);

    if (task == current)
        {
        status = "RUNNING";
        }
    else if (task->status == TASK_STATUS_READY)
        {
        status = "READY";
        }
    else if (task->status & TASK_STATUS_SUSPEND)
        {
        status = "SUSPEND";
        }
    else if (task->status & TASK_STATUS_PEND)
        {
        status = "PENDING";
        }
    else if (task->status & TASK_STATUS_DELAY)
        {
        status = "DELAY";
        }
    else
        {
        status = "UNKNOWN";
        }

    cmder_printf (cmder, " %-7s", status);

    if (task == current)
        {
        cmder_printf (cmder, " %8s\n", "NA");
        }
    else
        {
        cmder_printf (cmder, " %08x\n", __regset_get_pc (&task->regset));
        }

    return 0;
    }

static int __cmd_i (cmder_t * cmder, int argc, char * argv [])
    {
    size_t i;

    /* print out the header */

    cmder_printf (cmder, "NAME");

    for (i = 0; i < MAX_TASK_NAME_LEN - 5; i++)
        {
        cmder_printf (cmder, " ");
        }

    cmder_printf (cmder, " ENTRY    TASK_ID  O_PRI C_PRI SP_BASE  LOWEST   NOW      SP_TOP   STATUS   PC\n");

    for (i = 0; i < MAX_TASK_NAME_LEN - 1; i++)
        {
        cmder_printf (cmder, "=");
        }

    cmder_printf (cmder, " ======== ======== ===== ===== ======== ======== ======== ======== ======= ========\n");

    task_foreach (__task_show, (uintptr_t) cmder);

    return 0;
    }

CMDER_CMD_DEF ("i", "show task information", __cmd_i);

#endif

