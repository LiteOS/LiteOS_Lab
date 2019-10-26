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

#ifndef __LWIP_ARCH_CC_H__
#define __LWIP_ARCH_CC_H__

#include <stdlib.h>

#include <bug.h>
#include <warn.h>

#include <kprintf.h>

#ifndef __KEIL__
#define LWIP_TIMEVAL_PRIVATE 0

#include <sys/time.h>
#endif

#define LWIP_ERRNO_STDINCLUDE

#define LWIP_RAND() ((u32_t)rand())

/* different handling for unit test, normally not needed */

#ifdef LWIP_NOASSERT_ON_ERROR
#define LWIP_ERROR(message, expression, handler)            \
do                                                          \
    {                                                       \
    if (!(expression))                                      \
        {                                                   \
        handler;                                            \
        }                                                   \
    } while(0)
#endif

#define LWIP_PLATFORM_ASSERT(x)                             \
do                                                          \
    {                                                       \
    BUG (x);                                                \
    } while(0)

#define LWIP_PLATFORM_DIAG(x)                               \
do                                                          \
    {                                                       \
    WARN x;                                                 \
    } while(0)

#endif /* __LWIP_ARCH_CC_H__ */
