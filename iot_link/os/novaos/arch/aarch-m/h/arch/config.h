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

#ifndef __AARCH_M_CONFIG_H__
#define __AARCH_M_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>

#include <config.h>
#include <compiler.h>

#include "regset.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ALLOC_ALIGN                 8
#define STACK_ALIGN                 8

#define MIN_STACK_SIZE              0x200

/* typedefs */

typedef unsigned int pa_t;      /* TODO: */
typedef char       * va_t;      /* TODO: */

/* inlines */

static __always_inline size_t stack_size_get (size_t size)
    {
    size  = size < MIN_STACK_SIZE ? MIN_STACK_SIZE : size;
    size += sizeof (struct esp);

#ifdef CONFIG_FPU
    size += sizeof (struct esp_extended);
#endif

    return round_up (size, STACK_ALIGN);
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AARCH_M_CONFIG_H__ */

