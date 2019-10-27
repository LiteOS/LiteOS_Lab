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

/*
high level description
----------------------
0) overview

tick queue is a very common library used for:
1. task delay
2. task timeout when pending
3. timer

this library just provide the most basic implemention of a tick based delay
management, with proper callback supplied by the task library, timer library:

    +------------+-------+-------------------------------------+
    | task delay | timer | ipc timeout (sem, mutex, event ...) |
    +------------+-------+-------------------------------------+
    |                  tick queue (tick.c)                     |
    +----------------------------------------------------------+

different module using tick queue with different callbacks.

1) tick queue implementation
this module manages all of these different events by a struct of "tick_q_node",
a "tick_q_node" held the timeout callback, timeout callback argument, so when
a node timeout this module will just invoke the callback with the argument of
it

the timeout nodes are organized in a queue can ba called "delta queue", when we
insert 5 nodes with the value of "5 5 3 10 20", there will be:

    +------+    +---+    +---+    +---+    +---+    +----+
    | head | -> | 3 | -> | 2 | -> | 0 | -> | 5 | -> | 10 |
    +------+    +---+    +---+    +---+    +---+    +----+
                 (3)      (5)      (5)      (10)      (20)

as a real-time system, the delta queue is divided into 2^n sub-queues:

tick_qs [x] (x = 2^n):

    +------------+
    | tick_q [0] | -> ...
    +------------+
    | tick_q [1] | -> ...
    +------------+
    ~    ...     ~
    +------------+
    | tick_q [x] | -> ...
    +------------+

so when delay "n" ticks, the index is: (tick_count + n) & (2^x - 1).
and the value in the delay queue is: (n - 1) >> x
*/

#include <stdbool.h>

#include <config.h>
#include <common.h>
#include <task.h>
#include <irq.h>
#include <list.h>
#include <hal_timer.h>
#include <bug.h>
#include <tick.h>
#include <critical.h>
#include <errno.h>
#include <init.h>

#ifdef CONFIG_CMDER
#include <cmder.h>
#endif

#include <arch/interface.h>

#define TICK_Q_SHIFT            (CONFIG_NR_TICK_Q_SLOTS_ORDER)
#define TICK_Q_LISTS            (1 << TICK_Q_SHIFT)
#define TICK_Q_MASK             (TICK_Q_LISTS - 1)

/*
 * tick_count    - the system tick value
 * tick_next_act - the nearest tick that a timer or task will timeout
 */

static volatile union
    {
    uint32_t u32;
    uint64_t u64;
    } tick_count;

static dlist_t       tick_qs [TICK_Q_LISTS];
static hal_timer_t * systim = NULL;
static uint32_t      systim_freq;

#ifdef CONFIG_TICK_DEBUG
static uint32_t      shot_count = 0;
#endif

#ifdef CONFIG_TICKLESS
static uint32_t      systim_count_per_tick;
static uint32_t      systim_max_sleep_tick;

/*
 * jiff is defined as 1/n of a tick period, measured in the systim count value
 * and n is power of 2 (2 ^ systim_jiff_shift), for example:
 *
 *     4, 8, 16
 *
 * jiff is used for:
 *
 * 1) make sure the sleep enter logic is happen at the first 1/n of a tick, so
 *    the sleep logic can complete in one tick and will not cross two ticks
 * 2) when the system is waked up by a interrupt other than tick, the tick
 *    counter may be very near to the next tick baundary, the left operate code
 *    may cross the tick boundary:
 *
 *    |................|................|................|................|
 *                                     ^                 ^
 *                                     |                 |
 *                                     |             next tick
 *              system waked up here by interrupt other than systim
 *
 *    pick next tick as above, can avoid the crossing, and this will not effect
 *    the tick accuracy
 *
 * make sure the following execution can be done at a jiff time frame:
 *
 * 1) from tick_hander to idle, to the try sleep code if there is no other
 *    ready task
 * 2) a simple adjust count calculation after the system is waked up by
 *    interrupts other than systim and the following hal_timer_prepone
 *
 * in fact, this is not a problem, because the above two condition just have
 * very limit code, if it can not be done in 1/n tick, that means the system
 * HZ is too large, the system will spent too many time in systim handler,
 * do not use too large HZ value, especially for those system have very slow
 * cpu
 *
 * and jiff must greater than 1, at least 2
 */

static uint32_t      systim_jiff;
static const int     systim_jiff_shift = 4; // jiff = 1/16 tick

static uint32_t      sleep_ticks;

static void       (* __pfn_sleep_hook) (void) = NULL;
static void       (* __pfn_awake_hook) (void) = NULL;
#endif

/**
 * tick_tsc - system clock timestamp get
 *
 * return: the counter value
 */

unsigned long tick_tsc (void)
    {
    return hal_timer_counter (systim);
    }

/**
 * tick_tsc_period - get the sysclk timer period
 *
 * return: the period of the sysclk timer
 */

unsigned long tick_tsc_period (void)
    {
    return hal_timer_period (systim);
    }

/**
 * tick_tsc_freq - get the sysclk timer frequency
 *
 * return: the frequency of the sysclk timer
 */

unsigned long tick_tsc_freq (void)
    {
    return (unsigned long) systim_freq;
    }

static inline void __tick_q_add (struct tick_q_node * node, unsigned int ticks,
                                 dlist_t * tick_list)
    {
    dlist_t * itr;

    dlist_foreach (itr, tick_list)
        {
        struct tick_q_node * n = container_of (itr, struct tick_q_node, node);

        if (ticks < n->ticks_left)
            {
            n->ticks_left -= ticks;
            break;
            }

        ticks -= n->ticks_left;
        }

    node->ticks_left = ticks;

    /*
     * if tick_list is empty, itr is now just tick_list
     * if ticks is just the same with the node at the list end, itr is just
     * tick_list
     * if ticks is just small than a existed node, itr is now just the node
     *
     * for all of these conditions, just insert before itr
     */

    dlist_add_tail (itr, &node->node);
    }

/**
 * tick_q_add - add a tick queue node to tick queue
 * @node:  the tick queue to be added, usually come from a timer or task
 * @ticks: the max number of ticks the node stay in the tick queue
 * @pfn:   the timeout callback
 * @arg:   the argument for the timeout callback
 *
 * return: NA
 */

void tick_q_add (struct tick_q_node * node, unsigned int ticks,
                 void (* pfn) (struct tick_q_node *, uintptr_t), uintptr_t arg)
    {
    uint32_t idx = (tick_count.u32 + ticks) & TICK_Q_MASK;

    node->pfn = pfn;
    node->arg = arg;

    /*
     * ticks in the tick_qs should be:
     *
     *     if ((ticks & TICK_Q_MASK) == 0)
     *         ticks = (ticks >> TICK_Q_SHIFT) - 1;
     *     else
     *         ticks = (ticks >> TICK_Q_SHIFT) - 0;
     *
     * is equal to:
     *
     *     ticks = (ticks - 1) >> TICK_Q_SHIFT
     */

    ticks = (ticks - 1) >> TICK_Q_SHIFT;

    node->head = &tick_qs [idx];

    __tick_q_add (node, ticks, &tick_qs [idx]);
    }

/**
 * tick_q_del - remove a tick queue node from tick queue
 * @node:  the tick queue to be removed, usually come from a timer or task
 *
 * return: NA
 */

void tick_q_del (struct tick_q_node * node)
    {
    struct tick_q_node * next;
    bool                 last;

    /*
     * tick queues are a kind of delta queue, so when deleting a node in it,
     * the "ticks_left" member of the node after it must be adjusted if current
     * "ticks_left" is not '0'.
     * but when deleting the last node in a queue, this adjusting must be avoided
     * because this is no node after it, and its "next" is just pointing to the
     * queue head, a "dlist_t" data structure.
     * the tick queue is just a normal dual listed queue, so it is hard to check
     * if a node is the last, to deal with this situation, a "head" member is
     * added in "tick_q_node", just pointing to its queue head
     */

    last = (node->node.next == node->head);

    dlist_del (&node->node);

    if ((node->ticks_left == 0) || last)
        {
        return;
        }

    next = container_of (node->node.next, struct tick_q_node, node);

    next->ticks_left += node->ticks_left;
    }

static inline void tick_q_shot (void)
    {
    struct tick_q_node * node;
    dlist_t            * tick_list = &tick_qs [tick_count.u32 & TICK_Q_MASK];

    while (!dlist_empty (tick_list))
        {
        node = container_of (tick_list->next, struct tick_q_node, node);

        if (node->ticks_left != 0)
            {
            node->ticks_left--;
            break;
            }

        dlist_del (&node->node);

        node->pfn (node, node->arg);
        }
    }

static int __tick_shot_n (uintptr_t arg1, uintptr_t arg2)
    {
    unsigned int ticks = (unsigned int) arg1;

    (void) arg2;

#ifdef CONFIG_TICK_DEBUG
    shot_count++;
#endif

    tick_count.u64 += ticks;

    tick_q_shot ();

    /*
     * even for shotting N ticks, tick slice just need to be added once because
     * the system is just sleeped for at least (N-1) ticks
     */

    if (current != idle && ++current->tick_slices >= CONFIG_NR_RR_SLICING_TICKS)
        {
        current->tick_slices = 0;

        task_ready_q_del (current);
        task_ready_q_add (current);
        }

    return 0;
    }

/**
 * tick_shot_n - advance the tick queue by <n>
 * @ticks: the advanced tick numbers
 *
 * return: NA
 */

static inline void tick_shot_n (unsigned int ticks)
    {
    BUG_ON (do_critical (__tick_shot_n, (uintptr_t) ticks, 0) != 0,
            "critical queue full!");
    }

/**
 * tick_count_get - get the 32-bit tick count
 *
 * return: the 32-bit tick count
 */

uint32_t tick_count_get (void)
    {
    return tick_count.u32;
    }

/**
 * tick_count_get64 - get the 64-bit tick count
 *
 * return: the 64-bit tick count
 */

uint64_t tick_count_get64 (void)
    {
    return tick_count.u64;
    }

/**
 * __systim_handler - system timer interrupt handler
 */

static void __systim_handler (uintptr_t arg)
    {
    unsigned int ticks = 1;

    (void) arg;

#ifdef CONFIG_TICKLESS
    if (sleep_ticks != 0)
        {
        ticks = sleep_ticks;
        sleep_ticks = 0;
        }
#endif

    tick_shot_n (ticks);
    }

/**
 * tick_start - system tick start
 *
 * return: 0 on success, negtive value on error
 */

int tick_start (void)
    {
    int ret = hal_timer_connect (systim, __systim_handler, 0);

    if (ret != 0)
        {
        return ret;
        }

    return hal_timer_enable (systim, HAL_TIMER_MODE_REPEATED,
                             systim_freq / CONFIG_SYS_TICK_HZ);
    }

#ifdef CONFIG_TICKLESS

/**
 * __tick_idle_get - get the max number of idle ticks (can sleep)
 *
 * return: the max number of ticks can sleep
 */

static inline uint32_t __tick_idle_get (void)
    {
    uint32_t idle_ticks = UINT32_MAX;
    uint32_t i;
    uint32_t idx = (tick_count.u32 + 1) & TICK_Q_MASK;

    for (i = 1; i < TICK_Q_LISTS; i++, idx = (idx + 1) & TICK_Q_MASK)
        {
        struct tick_q_node * node;
        uint32_t             tick;

        if (dlist_empty (&tick_qs [idx]))
            {
            continue;
            }

        node = container_of (tick_qs [idx].next, struct tick_q_node, node);

        if (node->ticks_left == 0)
            {
            idle_ticks = i;
            break;
            }

        tick = (node->ticks_left << TICK_Q_SHIFT) + i;

        if (tick < idle_ticks)
            {
            idle_ticks = tick;
            }
        }

    return idle_ticks;
    }

/**
 * __do_sleep - system enter sleep
 *
 * return: true if the the system entered sleep mode and waked up by interrupts,
 *         false if the system not entered sleep mode
 */

static inline bool __do_sleep (uint32_t idle_ticks)
    {
    if (idle_ticks > systim_max_sleep_tick)
        {
        idle_ticks = systim_max_sleep_tick;
        }

    sleep_ticks = idle_ticks;

    if (hal_timer_postpone (systim, (sleep_ticks - 1) * systim_count_per_tick) != 0)
        {
        return false;
        }

    if (__pfn_sleep_hook != NULL)
        {
        __pfn_sleep_hook ();
        }

    cpu_idle_enter ();

    if (__pfn_awake_hook != NULL)
        {
        __pfn_awake_hook ();
        }

    return true;
    }

/**
 * __idle_for_seconds - try to check if the system is idle (no IO interrupt) for
 *                      <n> seconds
 *
 * return: true if no interrupt for <n> seconds
 */

static inline bool __idle_for_seconds (uint32_t n)
    {
    uint32_t i;

    for (i = 0; i < n * CONFIG_SYS_TICK_HZ; i++)
        {
        uint64_t last_tick = tick_count.u64;

        cpu_idle_enter ();

        /*
         * if system do not support instructions like 'wfi' or the system is
         * busy input/output 'last_tick == tick_count.u64' will be true
         */

        if (last_tick == tick_count.u64)
            {
            return false;
            }
        }

    return true;
    }

/**
 * tick_hook_set - set the sleep and awake callback for system sleep
 *
 * return: NA
 */

void tick_hook_set (void (* sleep) (void), void (* awake) (void))
    {
    __pfn_sleep_hook = sleep;
    __pfn_awake_hook = awake;
    }
#endif

/**
 * __try_sleep - try to sleep for power save
 *
 * return: NA
 */

static void __try_sleep (void)
    {
#ifdef CONFIG_TICKLESS
    unsigned long key;

    if (!__idle_for_seconds (1))
        {
        return;
        }

    key = int_lock ();
    task_lock ();

    /* make sure now is in the first jiff of a tick */

    while (likely (tick_tsc () < systim_jiff))
        {
        uint32_t      idle_ticks = __tick_idle_get ();
        uint32_t      tsc;
        unsigned long adj_count;

        /* if idle for the next n seconds, TODO: n should be configurable */

        if (unlikely (idle_ticks < 1 * CONFIG_SYS_TICK_HZ))
            {
            break;
            }

        if (unlikely (!__do_sleep (idle_ticks)))
            {
            break;
            }

        /* system may be waked up by any interrupt */

        int_restore (key);

        /*
         * interrupt is enabled, the interrupt will be handled, as the
         * task schedule is disabled so the interrupt will return here
         */

        key = int_lock ();

        /* if the system is waked up by systim, we will try to sleep again */

        if (sleep_ticks == 0)
            {
            continue;
            }

        /* if sleep_ticks is not 0, system is not waked up by systim */

        /*
         * every '|' is a tick boundary
         *
         * .________________tsc________________.
         * |                                   |     adj_count
         *   .------- elapsed_ticks -------.   |   .---------------.
         *   |                             |   |   |               |
         * |.......|.......|.......|.......|.......|.......|.......|
         *   ^                                 ^   ^               ^
         *   |                                 |   |               |
         *   +-- sleep from here              now  |       max sleep tick (7)
         *                                         +-- next tick after wake up
         *
         * sleep_ticks = 7
         * tick_shot_n (4);         // elapsed_ticks is 4
         * hal_timer_prepone (2);   // wake up system, next tick come at
         *                          // (sleep_ticks - elapsed_ticks - 1)
         *
         * for more detail information please refer to the comments for
         * <systim_jiff>
         */

        tsc = tick_tsc () + systim_jiff;

        adj_count = (sleep_ticks - tsc / systim_count_per_tick - 1) *
                    (systim_count_per_tick);

        hal_timer_prepone (systim, adj_count);

        sleep_ticks = 0;

        tick_shot_n (tsc / systim_count_per_tick);

        break;
        }

    int_restore (key);
    task_unlock ();
#else
    cpu_idle_enter ();
#endif
    }

/**
 * tick_lib_init - tick library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int tick_lib_init (void)
    {
    int      i;
    uint32_t systim_max_count;

    BUG_ON ((systim = hal_timer_get (CONFIG_TICK_TIMER_NAME)) == NULL,
            "Fail to get systim! CONFIG_TICK_TIMER_NAME:%s", CONFIG_TICK_TIMER_NAME);

    for (i = 0; i < TICK_Q_LISTS; i++)
        {
        dlist_init (&tick_qs [i]);
        }

    BUG_ON (hal_timer_feat_get (systim, &systim_max_count, &systim_freq) != 0,
            "Fail to get systim feature!");

#ifdef CONFIG_TICKLESS
    systim_count_per_tick = systim_freq / CONFIG_SYS_TICK_HZ;

    systim_jiff           = systim_count_per_tick >> systim_jiff_shift;

    /* see the comments for systim_jiff */

    BUG_ON (systim_jiff < 2, "CONFIG_SYS_TICK_HZ too high!");

    systim_max_sleep_tick = systim_max_count / systim_count_per_tick;

    BUG_ON (systim_max_sleep_tick == 0, "CONFIG_SYS_TICK_HZ too low!");
#endif

    task_idle_hook_set (__try_sleep);

    return 0;
    }

/*
 * tick init depends on driver, so init tick module at user group
 */

MODULE_INIT (postdriver, tick_lib_init);

#ifdef CONFIG_CMDER

static int __cmd_uptime (cmder_t * cmder, int argc, char * argv [])
    {
    uint32_t seconds = (uint32_t) (tick_count.u64 / CONFIG_SYS_TICK_HZ);
    uint32_t minites = (seconds / 60);
    uint32_t hours   = (minites / 60);

    cmder_printf (cmder, "system boot up tick: %lld\n", tick_count.u64);
    cmder_printf (cmder, "system boot up time: %02d:%02d:%02d\n",
                         hours, minites % 60, seconds % 60);

    cmder_printf (cmder, "system tick HZ:      %d\n", CONFIG_SYS_TICK_HZ);

#ifdef CONFIG_TICK_DEBUG
    cmder_printf (cmder, "shot_count:          %d\n", shot_count);
#endif

    return 0;
    }

CMDER_CMD_DEF ("uptime", "show the system running time", __cmd_uptime);

#endif
