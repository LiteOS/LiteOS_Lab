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

/* externs */

extern int main (void);

void __noreturn __rt_entry (void)
    {
    main ();
    task_exit ();

    while (1)
        {
        task_delay (CONFIG_SYS_TICK_HZ);
        }
    }

/*
 * app_entry - app entry point
 *
 * return: NA
 */

__section__ (vectors) __unused __naked void app_entry (void)
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
"       .extern __main                                                       \n"
#else
"       .extern _start                                                       \n"
#endif
"                                                                            \n"
#if     !defined (__SESA__) && !defined (__KEIL__)
"       @ copy data section, bss section will be zeroed in _start            \n"
"       @ for segger embedded studio for ARM will init .data, .bss in _start \n"
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
"      ldr      r0, =__rt_entry                                              \n"
#endif
"      bx       r0                                                           \n"
        );
    }

/* keep for linkage */

void reset_handler (void)
    {
    app_entry ();
    }

#ifdef __KEIL__

/*
 * To create a version of __user_setup_stackheap() that inherits sp from the
 * execution environment and does not have a heap:
 *
 *   In AArch32 state, set r0 and r2 to zero and return.
 *   In AArch64 state, set x0 and x2 to zero and return.
 */

__naked void __user_setup_stackheap (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       movs    r0, #0                                                       \n"
"       movs    r2, #0                                                       \n"
"       bx      lr                                                           \n"
        );
    }

/* disable the IDE supplied _fp_init */

void _fp_init (void)
    {
    }
#endif

#ifdef __SESA__

/* for linkage */

void _vectors (void)
    {
    }
#endif

