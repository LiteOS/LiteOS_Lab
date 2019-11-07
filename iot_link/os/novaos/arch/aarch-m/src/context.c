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

#include <config.h>
#include <compiler.h>
#include <task.h>

#define SCB_ICSR                    0xe000ed04
#define ICSR_PENDSVSET_OFFSET       28
#define ICSR_PENDSVSET              0x10000000

/**
 * sched_start - start up the task scheduling
 *
 * return: NA, will not return
 */

__naked void sched_start (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       .extern idle                                                         \n"
"       .extern current                                                      \n"
"                                                                            \n"
"       movs    r0, #2                                                       \n"
"       msr     control, r0                                                  \n"
"       dsb                                                                  \n"
"       isb                                                                  \n"
"                                                                            \n"
"       @ set current to idle and then send pendsv to launch the highest task\n"
"                                                                            \n"
"       ldr     r0, =idle                                                    \n"
"       ldr     r1, =current                                                 \n"
"       str     r0, [r1]                                                     \n"
"                                                                            \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       ldr     r1, [r0, #%c0]      @ get idle->regset.sp                    \n"
"       mov     sp, r1                                                       \n"
#else
"       ldr     sp, [r0, #%c0]      @ get idle->regset.sp                    \n"
#endif
"                                                                            \n"
"       add     sp, sp, #%c1                                                 \n"
"                                                                            \n"
"       cpsie   i                                                            \n"
"                                                                            \n"
"       bl      __pendsv                                                     \n"
"                                                                            \n"
"       ldr     r0, [r0, #%c2]      @ get idle entry                         \n"
"                                                                            \n"
"                                                                            \n"
"       blx     r0                  @ branch to the idle entry               \n"
"                                                                            \n"
"       b       .                   @ never reach here, idle never return    \n"
        :
        : "i" (offset_of (struct regset, sp)),
          "i" (sizeof (struct esp)),
          "i" (offset_of (task_t, entry))
        );
    }

/**
 * pendsv_handler - pendsv handler used for task switching
 *
 * return: NA
 */

__naked void pendsv_handler (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       .extern current                                                      \n"
"       .extern ready_q                                                      \n"
#ifdef  CONFIG_TASK_SWITCH_HOOK
"       .extern task_switch_hook                                             \n"
#endif
"                                                                            \n"
"       @                                                                    \n"
"       @ must be returning to task, interrupt needless to                   \n"
"       @ be disabled here                                                   \n"
"       @                                                                    \n"
"                                                                            \n"
"       mrs     r1, psp                                                      \n"
"                                                                            \n"
"       ldr     r2, =current                                                 \n"
"       ldr     r0, [r2]                                                     \n"
"                                                                            \n"
"       str     r1, [r0, #%c0]      @ save regset->sp                        \n"
"                                                                            \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"                                                                            \n"
"       @ stm not used for better interrupt response                         \n"
"                                                                            \n"
"       str     r4, [r0, #4 * 0]                                             \n"
"       str     r5, [r0, #4 * 1]                                             \n"
"       str     r6, [r0, #4 * 2]                                             \n"
"       str     r7, [r0, #4 * 3]                                             \n"
"                                                                            \n"
"       mov     r4, r8                                                       \n"
"       mov     r5, r9                                                       \n"
"       mov     r6, r10                                                      \n"
"       mov     r7, r11                                                      \n"
"                                                                            \n"
"       str     r4, [r0, #4 * 4]                                             \n"
"       str     r5, [r0, #4 * 5]                                             \n"
"       str     r6, [r0, #4 * 6]                                             \n"
"       str     r7, [r0, #4 * 7]                                             \n"
#else
"       stm     r0, {r4-r11}        @ IRQ can interrupt 'STM'                \n"
#endif
"       mrs     r1, control                                                  \n"
"       str     r1, [r0, #%c1]      @ save control                           \n"
#ifdef  CONFIG_FPU
"       str     lr, [r0, #%c2]      @ save exc_return                        \n"
"       tst     lr, #0x10           @ check if exc_return is extended        \n"
"       bne     0f                                                           \n"
"       add     r1, r0, #%c3                                                 \n"
"       vstm    r1, {d8-d15}                                                 \n"
"0:                                                                          \n"
#endif
"                                                                            \n"
"       ldr     r1, =ready_q        @ &ready_q.highest                       \n"
"                                                                            \n"
"       @ interrupt needless be disabled here                                \n"
"                                                                            \n"
"       ldr     r1, [r1]            @ next task                              \n"
"       str     r1, [r2]            @ current = ready_q->highest             \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       ldr     r4, [r1, #4 * 4]                                             \n"
"       ldr     r5, [r1, #4 * 5]                                             \n"
"       ldr     r6, [r1, #4 * 6]                                             \n"
"       ldr     r7, [r1, #4 * 7]                                             \n"
"                                                                            \n"
"       mov     r8, r4                                                       \n"
"       mov     r9, r5                                                       \n"
"       mov     r10, r6                                                      \n"
"       mov     r11, r7                                                      \n"
"                                                                            \n"
"       ldr     r4, [r1, #4 * 0]                                             \n"
"       ldr     r5, [r1, #4 * 1]                                             \n"
"       ldr     r6, [r1, #4 * 2]                                             \n"
"       ldr     r7, [r1, #4 * 3]                                             \n"
#else
"       ldm     r1, {r4-r11}        @ IRQ can interrupt 'LDM'                \n"
#endif
"                                                                            \n"
"       ldr     r2, [r1, #%c0]      @ save regset->sp                        \n"
"       msr     psp, r2                                                      \n"
"                                                                            \n"
"       ldr     r2, [r1, #%c1]      @ load regset->control                   \n"
"       msr     control, r2                                                  \n"
#ifdef  CONFIG_FPU
"       ldr     lr, [r1, #%c2]      @ load regset->exc_return                \n"
"       tst     lr, 0x10                                                     \n"
"       bne     0f                                                           \n"
"       add     r2, r1, #%c3                                                 \n"
"       vldm    r2, {d8-d15}                                                 \n"
"0:                                                                          \n"
#endif
#ifdef  CONFIG_AARCH_M_EXCLUSIVE
"       clrex                                                                \n"
#endif
#ifdef  CONFIG_TASK_SWITCH_HOOK
"       @ r0 = old, r1 = new                                                 \n"
"       ldr     r2,  =task_switch_hook                                       \n"
"       bx      r2                                                           \n"
#else
"       bx      lr                  @ do exception return                    \n"
#endif
        :
        : "i" (offset_of (struct regset, sp)),
          "i" (offset_of (struct regset, control))
#ifdef  CONFIG_FPU
        , "i" (offset_of (struct regset, exc_return)),
          "i" (offset_of (struct regset, d8))
#endif
        );
    }

/*
 * schedule - check if reshcedule is needed at the end of do_critical
 *
 * return: status
 *
 * NOTE: schedule is *ONLY* invoked in critical_exec after walking the critical
 *       queue, critical_q.in_critical should always be 'false', needless check
 *       here, and *NEVER* invoke this routine out critical_exec!
 */

__naked int schedule (int ra)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       .extern current                                                      \n"
"       .extern ready_q                                                      \n"
"       .extern task_lock_cnt                                                \n"
"                                                                            \n"
"       ldr     r1, =current                                                 \n"
"       ldr     r2, [r1]                                                     \n"
"       ldr     r3, =ready_q                                                 \n"
"       ldr     r3, [r3]                                                     \n"
"       cmp     r2, r3                                                       \n"
"       beq     0f                  @ no higher task                         \n"
"                                                                            \n"
"       ldr     r3, =task_lock_cnt                                           \n"
"       ldr     r3, [r3]                                                     \n"
"                                                                            \n"
#ifdef  CONFIG_AARCH_M_CBNZ_CBZ
"       cbnz    r3, 0f                                                       \n"
"       cbz     r2, 0f              @ r2 = current, pre-kernel, no schedule  \n"
#else
"       cmp     r3, #0                                                       \n"
"       bne     0f                                                           \n"
"       cmp     r2, #0              @ r2 = current, pre-kernel, no schedule  \n"
"       beq     0f                                                           \n"
#endif
"                                                                            \n"
"__pendsv:                          @ send pendsv                            \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       movs    r3, #1                                                       \n"
"       ldr     r2, =%c0                                                     \n"
"       lsls    r3, r3, %1                                                   \n"
#else
"       movw    r2, %0                                                       \n"
"       mov     r3, %2                                                       \n"
"       movt    r2, %1                                                       \n"
#endif
"       str     r3, [r2]                                                     \n"
"0:                                                                          \n"
"       bx      lr                                                           \n"
        :
#ifdef  CONFIG_AARCH_M_BASELINE
        : "i" (SCB_ICSR),
          "i" (ICSR_PENDSVSET_OFFSET)
#else
        : "i" (SCB_ICSR & 0xffff),
          "i" (SCB_ICSR >> 16),
          "i" (ICSR_PENDSVSET)
#endif
        );
    }
