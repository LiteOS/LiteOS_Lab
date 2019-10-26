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

#include <task.h>
#include <init.h>
#include <bug.h>

#ifdef CONFIG_MPU
int svc_tls_slot;
#endif

/**
 * svc_handler - system call handler
 *
 * return: NA, will not return
 */

__naked void svc_handler (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       .extern syscall_get_entry                                            \n"
"       .extern current                                                      \n"
#ifdef CONFIG_MPU
"       .extern svc_tls_slot                                                 \n"
"       .extern task_tls_set                                                 \n"
"       .extern task_tls_get                                                 \n"
#endif
"                                                                            \n"
"       mrs     r0, psp                                                      \n"
#ifdef CONFIG_MPU
"       mrs     r1, control                                                  \n"
"       @ shift control, keep nPRIV (bit [0]) only, and check if zero        \n"
"       lsls    r2, r1, #31                                                  \n"
"       bne     0f                                                           \n"
#endif
"       adr     r2, 1f                                                       \n"
"       str     r2, [r0, # 4 * 6]                                            \n"
"       bx      lr                                                           \n"
#ifdef CONFIG_MPU
"0:     adr     r2, 2f                                                       \n"
"       str     r2, [r0, # 4 * 6]                                            \n"
"       subs    r1, #1                                                       \n"
"       msr     control, r1                                                  \n"
"       bx      lr                                                           \n"
#endif
"       .balign 4                                                            \n"
"1:     @ SVCall from privileged mode                                        \n"
"       push    {r0-r4, lr}                                                  \n"
"       mov     r0, r12                                                      \n"
"       bl      syscall_get_entry                                            \n"
"                                                                            \n"
#ifdef AARCH_M_CBNZ_CBZ
"       cbz     r0, 9f                                                       \n"
#else
"       cmp     r0, #0                                                       \n"
"       beq     9f                                                           \n"
#endif
"       ldr     r1, [sp, #5 * 4]    @ get back lr                            \n"
"       mov     lr, r1                                                       \n"
"       str     r0, [sp, #5 * 4]    @ set syscall entry                      \n"
"       pop     {r0-r4, pc}         @ call syscall entry, and will return    \n"
"                                   @ to caller directly, will not be here   \n"
"9:                                                                          \n"
"       mvns    r0, r0              @ not supported syscall number           \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       str     r0, [sp]                                                     \n"
"       pop     {r0-r4, pc}                                                  \n"
#else
"       add     sp, sp, #4 * 4                                               \n"
"       pop     {r4, pc}                                                     \n"
#endif
"                                                                            \n"
#ifdef CONFIG_MPU
"       .balign 4                                                            \n"
"2:     @ SVCall from unprivileged mode                                      \n"
"       push    {r0-r4, lr}                                                  \n"
"       mov     r0, r12                                                      \n"
"       bl      syscall_get_entry                                            \n"
"                                                                            \n"
#ifdef AARCH_M_CBNZ_CBZ
"       cbz     r0, 2f                                                       \n"
#else
"       cmp     r0, #0                                                       \n"
"       beq     2f                                                           \n"
#endif
"                                                                            \n"
"       ldr     r2, [sp, #4 * 5]        @ get back user lr                   \n"
"       str     r0, [sp, #4 * 5]        @ save syscall handler               \n"
"       ldr     r0, =current                                                 \n"
"       ldr     r0, [r0]                                                     \n"
"       ldr     r1, =svc_tls_slot                                            \n"
"       ldr     r1, [r1]                                                     \n"
"       ldr     r3, =task_tls_set                                            \n"
"       blx     r3                                                           \n"
"                                                                            \n"
#ifdef  CONFIG_AARCH_M_BASELINE
"       adr     r0, 0f                                                       \n"
"       adds    r0, #1                                                       \n"
"       mov     lr, r0                                                       \n"
"       @ now the stack is just : {r0, r1, r2, r3, r4, syscall_handler }     \n"
"       pop     {r0-r4, pc}                                                  \n"
"       .balign 4                                                            \n"
"0:                                                                          \n"
#else
"       ldr     r12, [sp, #4 * 5]                                            \n"
"       pop     {r0-r3}                                                      \n"
"       adds    sp, #8                                                       \n"
"                                                                            \n"
"       blx     r12                                                          \n"
#endif
"                                                                            \n"
"       @ we will get the user return address by:                            \n"
"       @ task_tls_get (current, svc_tls_slot, &lr)                          \n"
"       @ so we save the syscall return value in r0, and make space for lr   \n"
"       @ and thus, we can just use 'pop {r0, pc}' to return to user land    \n"
"                                                                            \n"
"       push    {r0, r1}                                                     \n"
"                                                                            \n"
"       ldr     r0, =current                                                 \n"
"       ldr     r0, [r0]                                                     \n"
"       ldr     r1, =svc_tls_slot                                            \n"
"       ldr     r1, [r1]                                                     \n"
"       add     r2, sp, #4                                                   \n"
"       ldr     r3, =task_tls_get                                            \n"
"       blx     r3                                                           \n"
"                                                                            \n"
"       @ back to unprivileged                                               \n"
"                                                                            \n"
"       mrs     r1, control                                                  \n"
"       adds    r1, #1                                                       \n"
"       msr     control, r1                                                  \n"
"                                                                            \n"
"       @ return to user code                                                \n"
"                                                                            \n"
"       pop     {r0, pc}                                                     \n"
"2:                                                                          \n"
"       mrs     r1, control                                                  \n"
"       adds    r1, #1                                                       \n"
"       msr     control, r1                                                  \n"
"       b       9b                                                           \n"
#endif
        );
    }

#ifdef CONFIG_MPU

/**
 * svc_init - syscall module initialization
 *
 * user land lr is saved in task local storage, please refer to the svc_handler
 *
 * return: 0, or negtive value if error
 */

static int svc_init (void)
    {
    svc_tls_slot = task_tls_slot_alloc ();

    if (svc_tls_slot < 0)
        {
        WARN ("fail to allocate TLS slot!");
        return -1;
        }

    return 0;
    }

MODULE_INIT (postkernel, svc_init);

#endif
