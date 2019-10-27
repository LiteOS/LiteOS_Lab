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

#include <compiler.h>

/**
 * context_test - context_test (id)
 *
 * return: NA, will not return
 */

__naked void context_test (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       LSLS    r0, r0, #16                                                  \n"
"       LDR     r1, =0x0101                                                  \n"
"       LDR     r2, =0x0202                                                  \n"
"                                                                            \n"
"       SUB     sp, sp, #14 * 4                                              \n"
"       MOV     r3, sp                                                       \n"
"                                                                            \n"
"       ADDS    r0, r0, r1                                                   \n"
"       ADDS    r1, r0, r1                                                   \n"
"                                                                            \n"
"       STM     r3!, {r0, r1}            @ 0x00id0101, 0x00id0202            \n"
"                                                                            \n"
"       ADD     r0, r0, r2                                                   \n"
"       ADD     r1, r1, r2                                                   \n"
"       STM     r3!, {r0, r1}            @ 0x00id0303, 0x00id0404            \n"
"                                                                            \n"
"       ADD     r0, r0, r2                                                   \n"
"       ADD     r1, r1, r2                                                   \n"
"       STM     r3!, {r0, r1}            @ 0x00id0505, 0x00id0606            \n"
"                                                                            \n"
"       ADD     r0, r0, r2                                                   \n"
"       ADD     r1, r1, r2                                                   \n"
"       STM     r3!, {r0, r1}            @ 0x00id0707, 0x00id0808            \n"
"                                                                            \n"
"       ADD     r0, r0, r2                                                   \n"
"       ADD     r1, r1, r2                                                   \n"
"       STM     r3!, {r0, r1}            @ 0x00id0909, 0x00id1010            \n"
"                                                                            \n"
"       ADD     r0, r0, r2                                                   \n"
"       ADD     r1, r1, r2                                                   \n"
"       STM     r3!, {r0, r1}            @ 0x00id1111, 0x00id1212            \n"
"                                                                            \n"
"       ADD     r1, r1, r2                                                   \n"
"       STM     r3!, {r1, r2}            @ 0x00id1414, 0x00000202            \n"
"                                                                            \n"
"       ADD     r0, sp, #7 * 4                                               \n"
"                                                                            \n"
"       LDM     r0!, {r1-r6}                                                 \n"
"       MOV     r8, r1                                                       \n"
"       MOV     r9, r2                                                       \n"
"       MOV     r10, r3                                                      \n"
"       MOV     r11, r4                                                      \n"
"       MOV     r12, r5                                                      \n"
"       MOV     lr, r6                                                       \n"
"                                                                            \n"
"       MOV     r0, sp                                                       \n"
"       LDM     r0!, {r1-r7}                                                 \n"
"                                                                            \n"
"0:                                                                          \n"
"       LDR     r0, [sp, #0 * 4]                                             \n"
"       CMP     r0, r1                                                       \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #1 * 4]                                             \n"
"       CMP     r0, r2                                                       \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #2 * 4]                                             \n"
"       CMP     r0, r3                                                       \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #3 * 4]                                             \n"
"       CMP     r0, r4                                                       \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #4 * 4]                                             \n"
"       CMP     r0, r5                                                       \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #5 * 4]                                             \n"
"       CMP     r0, r6                                                       \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #6 * 4]                                             \n"
"       CMP     r0, r7                                                       \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #7 * 4]                                             \n"
"       CMP     r0, r8                                                       \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #8 * 4]                                             \n"
"       CMP     r0, r9                                                       \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #9 * 4]                                             \n"
"       CMP     r0, r10                                                      \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #10 * 4]                                            \n"
"       CMP     r0, r11                                                      \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #11 * 4]                                            \n"
"       CMP     r0, r12                                                      \n"
"       BNE     1f                                                           \n"
"                                                                            \n"
"       LDR     r0, [sp, #12 * 4]                                            \n"
"       CMP     r0, r14                                                      \n"
"       BEQ     0b                                                           \n"
"1:     @ never be here                                                      \n"
"       CPSID   i                                                            \n"
"       B       .                                                            \n"
        );
    }
