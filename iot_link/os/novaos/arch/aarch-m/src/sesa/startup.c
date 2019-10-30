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

#ifdef __SESA__

/**
 * _start - start up routine
 *
 * return: NA, will not return
 */

__naked void _start (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       .extern __SEGGER_init_table__                                        \n"
"       .extern __rt_entry                                                   \n"
"                                                                            \n"
"       @ stack is set up by reset_handler already                           \n"
"                                                                            \n"
"       @ run SEGGER init table                                              \n"
"                                                                            \n"
"       ldr     r7, =__SEGGER_init_table__                                   \n"
"0:                                                                          \n"
#ifdef  CONFIG_AARCH_M_MAINLINE
"       ldr     r0, [r7], #4                                                 \n"
#else
"       ldr     r0, [r7]                                                     \n"
"       adds    r7, #4                                                       \n"
#endif
#ifdef  CONFIG_AARCH_M_CBNZ_CBZ
"       cbz     r0, 1f                                                       \n"
#else
"       cmp     r0, #0                                                       \n"
"       beq     1f                                                           \n"
#endif
"       blx     r0                                                           \n"
"       b       0b                                                           \n"
"                                                                            \n"
"1:     ldr     r0, =__rt_entry                                              \n"
"       blx     r0                                                           \n"
        );
    }

/**
 * __SEGGER_init_copy - segger init copy routine
 *
 * return: NA
 */

__naked void __SEGGER_init_copy (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       ldm     r7!, {r0, r1, r2}       @ r0 = dst, r1 = src, r2 = len       \n"
#ifdef  CONFIG_AARCH_M_CBNZ_CBZ
"       cbz     r2, 1f                                                       \n"
#else
"       cmp     r2, #0                                                       \n"
"       beq     1f                                                           \n"
#endif
"0:     subs    r2, #1                                                       \n"
"       ldrb    r3, [r1, r2]                                                 \n"
"       strb    r3, [r0, r2]                                                 \n"
"       bhi     0b                                                           \n"
"1:                                                                          \n"
"       bx      lr                                                           \n"
        );
    }

/**
 * __SEGGER_init_zero - segger init zero routine
 *
 * return: NA
 */

__naked void __SEGGER_init_zero (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       ldm     r7!, {r0, r1}           @ r0 = dst, r1 = len                 \n"
#ifdef  CONFIG_AARCH_M_CBNZ_CBZ
"       cbz     r1, 1f                                                       \n"
#else
"       cmp     r1, #0                                                       \n"
"       beq     1f                                                           \n"
#endif
"       movs    r2, #0                                                       \n"
"0:     subs    r1, #1                                                       \n"
"       strb    r2, [r0, r1]                                                 \n"
"       bhi     0b                                                           \n"
"1:                                                                          \n"
"       bx      lr                                                           \n"
        );
    }

/**
 * __aeabi_read_tp - get tls bss section
 *
 * return: NA
 */

__naked void __aeabi_read_tp (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       .extern __tbss_start__                                               \n"
"                                                                            \n"
"       ldr     r0, =__tbss_start__ - 8                                      \n"
"       bx      lr                                                           \n"
        );
    }

#endif

