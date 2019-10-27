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

#ifndef __AARCH_M_SYNC_H__
#define __AARCH_M_SYNC_H__

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * mb - read write memory barrier
 *
 * return: NA
 */

static __always_inline void mb (void)
    {
    __asm__ __volatile__ ("dmb #0xf" : : : "memory");
    }

/**
 * rmb - read memory barrier
 *
 * return: NA
 */

static __always_inline void rmb (void)
    {
    __asm__ __volatile__ ("dmb #0xf" : : : "memory");
    }

/**
 * wmb - write memory barrier
 *
 * return: NA
 */

static __always_inline void wmb (void)
    {
    __asm__ __volatile__ ("dmb #0xf" : : : "memory");
    }

/**
 * dsb - data synchronization barrier
 *
 * return: NA
 */

static __always_inline void dsb (void)
    {
    __asm__ __volatile__ ("dsb #0xf" : : : "memory");
    }

/**
 * isb - instruction synchronization barrier
 *
 * return: NA
 */

static __always_inline void isb (void)
    {
    __asm__ __volatile__ ("isb #0xf" : : : "memory");
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AARCH_M_SYNC_H__ */
