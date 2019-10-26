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
#include <irq.h>
#include <init.h>

#ifdef CONFIG_PROFILE
#include <profile.h>
#endif

#ifdef __KEIL__
#include <rt_misc.h>
#endif

/* externs */

/* macros */

/*
 * give user `main.c` and `main ()` as there entry point, create main task for
 * them
 *
 * KEIL: reset_handler -> __main -> __rt_entry
 * SESA: reset_handler -> _start -> __rt_entry
 * GNUC: reset_handler -> __rt_entry
 */

void __noreturn __rt_entry (void)
    {
#ifdef __KEIL__
    __rt_lib_init (NULL, NULL);
#endif
    kernel_init ();
    }

#ifdef __KEIL__

/* disable the IDE supplied _fp_init */

void _fp_init (void)
    {
    }

#endif

/*
 * reset_handler - system reset handler
 *
 * return: NA
 */

__naked void reset_handler (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
#if     !defined (__SESA__) && !defined (__KEIL__)
"       .extern __text_end__                                                 \n"
"       .extern __data_start__                                               \n"
"       .extern __data_end__                                                 \n"
"       .extern __bss_start__                                                \n"
"       .extern __bss_end__                                                  \n"
"       .extern __rt_entry                                                   \n"
"                                                                            \n"
#endif
#ifdef  __KEIL__
"       .equ    __stack_end__, Image$$ARM_LIB_STACKHEAP$$ZI$$Limit           \n"
"                                                                            \n"
"       .extern __main                                                       \n"
#endif
#ifdef  __SESA__
"       .extern _start                                                       \n"
#endif
"                                                                            \n"
"       .extern __stack_end__                                                \n"
"                                                                            \n"
"       @ prevent any interrupt come before system ready                     \n"
"                                                                            \n"
"       cpsid   i                                                            \n"
"                                                                            \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       ldr     r0, =__stack_end__                                           \n"
"       mov     sp, r0                                                       \n"
#else
"       ldr     sp, =__stack_end__                                           \n"
#endif
"                                                                            \n"
#if     !defined (__SESA__) && !defined (__KEIL__)
"       @ segger embedded studio for ARM will init .data, .bss in _start     \n"
"                                                                            \n"
"       @ initialize data section                                            \n"
"                                                                            \n"
"       ldr     r1, =__text_end__                                            \n"
"       ldr     r2, =__data_start__                                          \n"
"       ldr     r3, =__data_end__                                            \n"
"                                                                            \n"
"       subs    r3, r2                                                       \n"
"       bls     1f                                                           \n"
"0:                                                                          \n"
"       subs    r3, #4                                                       \n"
"       ldr     r0, [r1, r3]                                                 \n"
"       str     r0, [r2, r3]                                                 \n"
"       bhi     0b                                                           \n"
"1:                                                                          \n"
"                                                                            \n"
"       @ clear bss section                                                  \n"
"                                                                            \n"
"       ldr     r2, =__bss_start__                                           \n"
"       ldr     r3, =__bss_end__                                             \n"
"                                                                            \n"
"       subs    r3, r2                                                       \n"
"       bls     1f                                                           \n"
"       movs    r0, #0                                                       \n"
"0:                                                                          \n"
"       subs    r3, #4                                                       \n"
"       str     r0, [r2, r3]                                                 \n"
"       bhi     0b                                                           \n"
"1:                                                                          \n"
#endif
"                                                                            \n"
"       @ KEIL: reset_handler -> __main -> __rt_entry                        \n"
"       @ SESA: reset_handler -> _start -> __rt_entry                        \n"
"       @ GNUC: reset_handler -> __rt_entry                                  \n"
"                                                                            \n"
#if    defined (__KEIL__)
"      ldr      r0, =__main                                                  \n"
#elif  defined (__SESA__)
"      ldr      r0, =_start                                                  \n"
#else
"      ldr      r0, =__libc_init_array                                       \n"
"      blx      r0                                                           \n"
"      ldr      r0, =__rt_entry                                              \n"
#endif
"      bx       r0                                                           \n"
       );
    }

/**
 * irq_handler - irq handler when using irq dispatching
 *
 * return: NA
 */

__naked void irq_handler (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"       .extern hal_int_dispatch                                             \n"
#ifdef CONFIG_PROFILE
"       .extern profile_enter                                                \n"
"       .extern profile_exit                                                 \n"
"                                                                            \n"
"       push    {r0, lr}                                                     \n"
"       ldr     r0, =profile_sys_enter                                       \n"
"       blx     r0                                                           \n"
#endif
"                                                                            \n"
"       mrs     r0, ipsr                                                     \n"
"       subs    r0, #16                                                      \n"
#ifdef  CONFIG_PROFILE
"       ldr     r1, =hal_int_dispatch                                        \n"
"       blx     r1                                                           \n"
#else
#ifdef  CONFIG_AARCH_M_BASELINE
"       ldr     r1, =hal_int_dispatch                                        \n"
"       bx      r1                                                           \n"
#else
"       ldr     pc, =hal_int_dispatch                                        \n"
#endif
#endif

        /*
         * needless check task switch for cortex-m (due to pendsv) and the
         * interrupt handler will never return here, it will return to task
         * directly
         */

#ifdef CONFIG_PROFILE
"       ldr     r0, =profile_sys_exit                                        \n"
"       blx     r0                                                           \n"
#ifdef  CONFIG_AARCH_M_EXCLUSIVE
"       clrex                                                                \n"
#endif
"       pop     {r0, pc}                                                     \n"
#endif
        );
    }

/**
 * exc_handler - exception handler for all exceptions
 *
 * return: NA
 */

__naked void exc_handler (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"       .extern arch_exc_handler                                             \n"
"                                                                            \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       mov     r0, r8                                                       \n"
"       mov     r1, r9                                                       \n"
"       mov     r2, r10                                                      \n"
"       mov     r3, r11                                                      \n"
"                                                                            \n"
"       push    {r0-r3, lr}                                                  \n"
"       push    {r4-r7}                                                      \n"
"                                                                            \n"
"       mov     r0, lr                                                       \n"
"       movs    r1, #1 << 2                                                  \n"
"       tst     r0, r1                                                       \n"
"       mrs     r0, psp                                                      \n"
"       bne     0f                                                           \n"
"       mov     r0, sp                                                       \n"
"       adds    r0, r0, #4 * 9                                               \n"
#else
"       mov     r0, sp                                                       \n"
"       sub     sp, sp, #6 * 4          @ reserve space shcsr ~ bfar         \n"
"       push    {r4-r11, lr}                                                 \n"
"                                                                            \n"
"       tst     lr, #1 << 2                                                  \n"
"       beq     0f                                                           \n"
"                                                                            \n"
"       mrs     r0, psp                                                      \n"
#endif
"0:                                                                          \n"
"       push    {r0}                                                         \n"
"       mov     r0, sp                                                       \n"
"       bl      arch_exc_handler                                             \n"
"       add     sp, sp, #4                                                   \n"
#ifdef CONFIG_AARCH_M_BASELINE
"       pop     {r4-r7}                                                      \n"
"       pop     {r0-r3}                                                      \n"
"       mov     r8, r0                                                       \n"
"       mov     r9, r1                                                       \n"
"       mov     r10, r2                                                      \n"
"       mov     r11, r3                                                      \n"
"       pop     {pc}                                                         \n"
#else
"       pop     {r4-r11, lr}                                                 \n"
"       add     sp, sp, #6 * 4                                               \n"
"       bx      lr                                                           \n"
#endif
        );
    }

/**
 * systick_handler - default SysTick handler, as some SoC do not have systick
 *
 * return: NA
 */

__naked __weak void systick_handler (void)
    {
    __asm__ __volatile__
        (
"       b       .                                                            \n"
        );
    }

