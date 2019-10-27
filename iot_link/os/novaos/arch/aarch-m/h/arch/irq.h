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

#ifndef __AARCH_M_IRQ_H__
#define __AARCH_M_IRQ_H__

#include <stdbool.h>

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * int_lock - disable irq
 *
 * return: the original primask
 */

static __always_inline unsigned long int_lock (void)
    {
    unsigned long flags;

    __asm__ __volatile__
        (
"       mrs     %0, primask                                                  \n"
"       cpsid   i                                                            \n"
        : "=r" (flags)
        :
        : "memory"
        );

    return flags;
    }

/*
 * int_unlock - enable irq
 *
 * return: the original primask
 */

static __always_inline unsigned long int_unlock (void)
    {
    unsigned long flags;

    __asm__ __volatile__
        (
"       mrs     %0, primask                                                  \n"
"       cpsie   i                                                            \n"
        : "=r" (flags)
        :
        : "memory"
        );

    return flags;
    }

/*
 * int_restore - restore primask
 * @flags: the original primask value
 *
 * return: NA
 */

static __always_inline void int_restore (unsigned long flags)
    {
    __asm__ __volatile__
        (
"       msr     primask, %0                                                  \n"
        :
        : "r" (flags)
        : "memory"
        );
    }

/**
 * int_locked - return if interrupt locked
 *
 * return: true or false
 */

static __always_inline bool int_locked (void)
    {
    unsigned long flags;

    __asm__ __volatile__
        (
"       mrs     %0, primask                                                  \n"
        : "=r" (flags)
        );

    return flags != 0;
    }

/**
 * int_context_arch - return if interrupt context
 *
 * return: true or false
 */

static __always_inline bool int_context_arch (void)
    {
#if   defined (__CC_ARM)
	register uint32_t ipsr __asm ("ipsr");
#elif defined (__GNUC__) || defined (__clang__)
    unsigned int ipsr;

	__asm__ __volatile__ ("mrs %0, ipsr" : "=r" (ipsr));
#else
#error "unsurpported tool!"
#endif

    return ipsr != 0;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AARCH_M_IRQ_H__ */
