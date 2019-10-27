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
#include <critical.h>
#include <init.h>

#include <arch/spr.h>

/*
 * cpu_init - cpu specific initialization
 *
 * return: NA
 */

static int cpu_init (void)
    {
#ifdef CONFIG_HAS_VTOR
    extern char __section_start__ (vectors) [];

    scb->vtor   = (uint32_t) __section_start__ (vectors);
#endif

    /* set the priority of pendsv and systick to lowest */

    scb->shpr3  = 0xffff0000;

#ifdef CONFIG_SVC

    /* set the priority of svcall to lowest */

    scb->shpr2  = 0xff000000;
#endif

#ifdef CONFIG_FPU
    scb->cpacr  = 0x00f00000;
#endif

#ifdef CONFIG_AARCH_M_MAINLINE
    scb->ccr   |= CCR_DIV_0_TRP;
    scb->shcsr |= SHCSR_USGFAULTENA | SHCSR_BUSFAULTENA | SHCSR_MEMFAULTENA;
#endif

    return 0;
    }

MODULE_INIT (cpu, cpu_init);

/*
 * mem_try_arch - memory try arch support enter routine
 * @dst:   the address write to
 * @src:   the address read from
 * @order: unit size (1 << order bytes)
 *
 * return: 0 on success, negtive value on error
 */

__naked int mem_try_arch (void * dst, void * src, int order)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       @ `adr r3, 0f` do not work with SESA, r3 will be the address of 0f -2\n"
"       @ so, use `ldr r3, =0f` instead                                      \n"
"       ldr     r3, =0f                                                      \n"
"       ldrb    r2, [r3, r2]                                                 \n"
"       add     r2, r3                                                       \n"
"       bx      r2                                                           \n"
"0:     .byte   (8f  - 0b) | 1                                               \n"
"       .byte   (16f - 0b) | 1                                               \n"
"       .byte   (32f - 0b) | 1                                               \n"
"       .byte   (64f - 0b) | 1                                               \n"
#else
"       tbb     [pc, r2]                                                     \n"
"0:     .byte   (8f  - 0b) / 2                                               \n"
"       .byte   (16f - 0b) / 2                                               \n"
"       .byte   (32f - 0b) / 2                                               \n"
"       .byte   (64f - 0b) / 2                                               \n"
#endif
"8:     ldrb    r1, [r1]                                                     \n"
"       strb    r1, [r0]                                                     \n"
"       b       0f                                                           \n"
"16:    ldrh    r1, [r1]                                                     \n"
"       strh    r1, [r0]                                                     \n"
"       b       0f                                                           \n"
"32:    ldr     r1, [r1]                                                     \n"
"       str     r1, [r0]                                                     \n"
"       b       0f                                                           \n"
"64:    ldm     r1!, {r2, r3}                                                \n"
"       stm     r0!, {r2, r3}                                                \n"
"0:                                                                          \n"
"       isb                                                                  \n"
"       dsb                                                                  \n"
"       nop                                                                  \n"
"                                                                            \n"
"       movs    r0, #0                  @ OK                                 \n"
"                                                                            \n"
" @                                                                          \n"
" @ mem_try_arch_end - memory try arch support exit routine                  \n"
" @                                                                          \n"
" @ return: NA                                                               \n"
" @                                                                          \n"
"       .global mem_try_arch_end                                             \n"
"mem_try_arch_end:                                                           \n"
"                                                                            \n"
"       bx      lr                                                           \n"
        );
    }

/*
 * cpu_idle_enter - wait for interrupt and entery low power mode
 *
 * return: NA
 */

__naked void cpu_idle_enter (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
#ifdef  CONFIG_WFI
"       wfi                                                                  \n"
#endif
"       isb                                                                  \n"
"       dsb                                                                  \n"
"       bx      lr                                                           \n"
        );
    }

/**
 * critical_exec - exec a job and then walk through the critical queue
 *
 * return: return value of the job
 *
 * NOTE: critical stack can only be used when all the following conditions are
 *       true:
 * 1) task context (not interrupt context)
 * 2) critical_q.in_critical == true
 *
 * see also critical_exec the portable version in critical.c
 */

__naked int critical_exec (int (* job) (uintptr_t, uintptr_t),
                           uintptr_t arg1, uintptr_t arg2)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       .extern critical_q                                                   \n"
"       .extern errno_get                                                    \n"
#ifdef CONFIG_PROFILE
"       .extern profile_sys_enter                                            \n"
"       .extern profile_sys_exit                                             \n"
#endif
#ifdef  CONFIG_CRITICAL_STACK_CHECK
"       .extern __bug                                                        \n"
#endif
"                                                                            \n"
"       push    {r3-r7, lr}                                                  \n"
"                                                                            \n"
"       ldr     r4, =critical_q + %c1       @ r4 = &critical_q.in_critical   \n"
"                                                                            \n"
"       ldr     r3, [r4]                    @ get critical_q.in_critical     \n"
#ifdef  CONFIG_AARCH_M_CBNZ_CBZ
"       cbnz    r3, 3f                      @ if already in critical, add job\n"
#else
"       cmp     r3, #0                                                       \n"
"       bne     3f                          @ if already in critical, add job\n"
#endif
"                                                                            \n"
"       mrs     r5, ipsr                    @ get ipsr in r5                 \n"
"       mov     r6, sp                      @ save original sp in r6         \n"
"                                                                            \n"
"       @ adjust arguments in registers according to the ABI                 \n"
"                                                                            \n"
"       mov     r3, r0                                                       \n"
"       mov     r0, r1                                                       \n"
"       mov     r1, r2                                                       \n"
"                                                                            \n"
"       @ enter critical                                                     \n"
"                                                                            \n"
"       movs    r2, #1                                                       \n"
"       str     r2, [r4]                    @ critical_q.in_critical = true  \n"
"                                                                            \n"
"       @ do not use critical stack in int_context                           \n"
"                                                                            \n"
#ifdef  CONFIG_AARCH_M_CBNZ_CBZ
"       cbnz    r5, 0f                                                       \n"
#else
"       cmp     r5, #0                                                       \n"
"       bne     0f                                                           \n"
#endif
"       mov     sp, r4                      @ set up the critical stack      \n"
"                                           @ if not IRQ context             \n"
"0:     blx     r3                          @ job (arg1, arg2)               \n"
"       mov     r7, r0                      @ save return value in r7        \n"
"                                                                            \n"
"       ldr     r5, [r4, #4]                @ r5 = critical_q.head_idx       \n"
"                                                                            \n"
"0:     mrs     r3, primask                 @ r3 = int_lock ()               \n"
"       cpsid   i                                                            \n"
"       ldr     r0, [r4, #8]                @ r0 = critical_q.tail_idx       \n"
"       subs    r0, r5                                                       \n"
"       bne     1f                          @ not empty? unlikely            \n"
"                                                                            \n"
"       @ no more job in the queue, return                                   \n"
"                                                                            \n"
"       str     r0, [r4]                    @ critical_q.in_critical = false \n"
"       mov     sp, r6                      @ restore original sp            \n"
"                                                                            \n"
"       msr     primask, r3                 @ int_restore (r3)               \n"
"                                                                            \n"
#ifdef  CONFIG_CRITICAL_STACK_CHECK
"       @ check if critical stack overflow                                   \n"
"       ldr     r1, =critical_q                                              \n"
"       ldr     r2, [r1]                                                     \n"
"       ldr     r3, =%c3                    @ CRITICAL_MAGIC low  32bit      \n"
"       cmp     r2, r3                                                       \n"
"       bne     2f                                                           \n"
"       ldr     r2, [r1, #4]                                                 \n"
"       ldr     r3, =%c4                    @ CRITICAL_MAGIC high 32bit      \n"
"       cmp     r2, r3                                                       \n"
"       bne     2f                                                           \n"
#endif
"                                                                            \n"
"       mov     r0, r7                      @ get back return value          \n"
"       bl      schedule                    @ check reschedule               \n"
"                                                                            \n"
"       pop     {r3-r7, pc}                 @ return                         \n"
"                                                                            \n"
"       @ critical queue is not empty, call one entry                        \n"
"1:     msr     primask, r3                 @ int_restore (r3)               \n"
#ifdef CONFIG_PROFILE
"       ldr     r3, =profile_sys_enter                                       \n"
"       blx     r3                                                           \n"
#endif
#ifdef  CONFIG_AARCH_M_BASELINE
"       movs    r3, %0 - 1                                                   \n"
"       ands    r3, r5                      @ r3 = head_idx & mask           \n"
"       lsls    r3, #4                                                       \n"
"       add     r3, r4                      @ r3 = &jobs [idx] - 0x10        \n"
#else
"       and     r3, r5, %0 - 1              @ r3 = head_idx & mask           \n"
"       add     r3, r4, r3, lsl #4          @ r3 = &jobs [idx] - 0x10        \n"
#endif
"       ldr     r0, [r3, #0x14]             @ get arg1                       \n"
"       ldr     r1, [r3, #0x18]             @ get arg2                       \n"
"       ldr     r2, [r3, #0x10]             @ get pfn                        \n"
"       adds    r5, #1                      @ head_idx++                     \n"
"       str     r5, [r4, #4]                @ critical_q.head_idx = head_idx \n"
"       blx     r2                          @ invoke the job                 \n"
#ifdef CONFIG_PROFILE
"       ldr     r3, =profile_sys_exit                                        \n"
"       blx     r3                                                           \n"
#endif
"       b       0b                                                           \n"
"                                                                            \n"
#ifdef  CONFIG_CRITICAL_STACK_CHECK
"       @ critical stack overflow                                            \n"
"2:     ldr     r0, =%5                                                      \n"
"       ldr     r1, =__bug                                                   \n"
"       blx     r1                                                           \n"
"       b       .                                                            \n"
#endif

/**
 * __critical_q_add - add a job to the critical queue
 * @job:  the job routine
 * @arg1: the first argument
 * @arg2: the second argument
 *
 * return: 0 on success, -1 if queue full
 *
 * see also __critical_q_add the portable version in critical.c
 */

"3:     mrs     r6, primask                                                  \n"
"                                                                            \n"
"       @                                                                    \n"
"       @ it is safe to load head_idx here (out of the irq lock window):     \n"
"       @   1) we must be in interrupt context                               \n"
"       @   2) a task, or a lower priority interrupt is handing the queue    \n"
"       @   3) no one else (a higher irq) will be able to handle the queue   \n"
"       @      even it may be raised and interrupt this routine              \n"
"       @ so the head_idx will nevered be changed here                       \n"
"       @                                                                    \n"
"                                                                            \n"
"       ldr     r3, [r4, #4]                @ r3 = critical_q.head_idx       \n"
"       adds    r3, %0                      @ r3 = r3 + nr_elements          \n"
"                                                                            \n"
"       cpsid   i                           @ r6 = int_lock ()               \n"
"       ldr     r5, [r4, #8]                @ r5 = critical_q.tail_idx       \n"
"       subs    r3, r5                      @ r3 = head_idx + nr_elements -  \n"
"                                           @      tail_idx                  \n"
"                                           @    = free slots left in queue  \n"
"       beq     0f                          @ no free slots (full), unlikely \n"
"                                                                            \n"
"       adds    r3, r5, #1                                                   \n"
"       str     r3, [r4, #8]                @ critical_q.tail_idx++          \n"
"       dsb                                                                  \n"
"       msr     primask, r6                 @ int_restore (r6)               \n"
"                                                                            \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       movs    r3, %0 - 1                                                   \n"
"       ands    r3, r5                      @ r3 = idx = head_idx & mask     \n"
"       lsls    r3, #4                                                       \n"
"       add     r3, r4                      @ r3 = &jobs [idx] - 0x10        \n"
#else
"       ands    r5, %0 - 1                  @ r5 = idx = head_idx & mask     \n"
"       add     r3, r4, r5, lsl #4          @ r3 = &jobs [idx] - 0x10        \n"
#endif
"       @ set up the queue                                                   \n"
"       str     r0, [r3, #0x10]             @ set pfn                        \n"
"       str     r1, [r3, #0x14]             @ set arg1                       \n"
"       str     r2, [r3, #0x18]             @ set arg2                       \n"
"                                                                            \n"
"       @ return 0                                                           \n"
"       movs    r0, #0                                                       \n"
"       pop     {r3-r7, pc}                 @ return 0                       \n"
"                                                                            \n"
"       @ critical_q is full, set errno and return -1                        \n"
"0:     msr     primask, r6                                                  \n"
"       ldr     r1, =errno_get                                               \n"
"       blx     r1                                                           \n"
"       ldr     r1, =%c2                                                     \n"
"       str     r1, [r0]                                                     \n"
"       movs    r0, #0                                                       \n"
"       mvns    r0, r0                                                       \n"
"       pop     {r3-r7, pc}                 @ return -1                      \n"
        :
        : "i" (CONFIG_NR_CRITICAL_JOB_Q_SLOTS),
#ifdef CONFIG_CRITICAL_STACK_CHECK
          "i" (CONFIG_CRITICAL_STACK_SIZE + sizeof (uint64_t)),
#else
          "i" (CONFIG_CRITICAL_STACK_SIZE),
#endif
          "i" (ERRNO_CRITICAL_QUEUE_FULL),
          "i" (CRITICAL_MAGIC & 0xffffffffull),
          "i" (CRITICAL_MAGIC >> 32)
#ifdef  CONFIG_CRITICAL_STACK_CHECK
        , "i" ("critical stack overflow!")
#endif
        );
    }

/*
 * critical_exec_on_csp - do critical job on critical stack
 *
 * return: the job executive result
 */

__naked int critical_exec_on_csp (int (* job) (uintptr_t, uintptr_t),
                                  uintptr_t arg1, uintptr_t arg2)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       push    {r6, lr}                                                     \n"
"       mov     r6, sp                      @ save original sp in r6         \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       ldr     r3, =critical_q + %c0       @ r3 = &critical_q               \n"
"       mov     sp, r3                      @ sp = &critical_q               \n"
#else
"       ldr     sp, =critical_q + %c0       @ sp = &critical_q               \n"
#endif
"                                                                            \n"
"       mov     r3, r0                                                       \n"
"       mov     r0, r1                                                       \n"
"       mov     r1, r2                                                       \n"
"                                                                            \n"
"       blx     r3                                                           \n"
"                                                                            \n"
"       mov     sp, r6                      @ restore original sp            \n"
"       pop     {r6, pc}                                                     \n"
        :
#ifdef CONFIG_CRITICAL_STACK_CHECK
        : "i" (CONFIG_CRITICAL_STACK_SIZE + sizeof (uint64_t))
#else
        : "i" (CONFIG_CRITICAL_STACK_SIZE)
#endif
        );
    }

