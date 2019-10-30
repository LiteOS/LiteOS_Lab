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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stddef.h>             /* for size_t */
#include <stdint.h>

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * offset_of - caculate the offset for a member in a struct
 * @t: the type of the container struct this is embedded in
 * @m: the name of the member within the struct
 */

#define offset_of(t, m)         ((uintptr_t) &((t *) 0)->m)

/*
 * container_of - cast a member of a structure out to the containing structure
 * @p: the pointer to the member
 * @t: the type of the container struct this is embedded in
 * @m: the name of the member within the struct
 */

#define container_of(p, t, m)   ((t *) (((char *) (p)) - (offset_of (t, m))))

/*
 * round_up - make a value to be aligned forward
 * @x: the value to be aligned
 * @a: the alignment value
 */

#define round_up(x, a)          (((uintptr_t) (x) + ((uintptr_t) (a) - 1)) & ~((uintptr_t) (a) - 1))

/*
 * round_down - make a value to be aligned backward
 * @x: the value to be aligned
 * @a: the alignment value
 */

#define round_down(x, a)        ((uintptr_t) (x) & ~((uintptr_t) (a) - 1))

/*
 * align_at - check if a value is aligned
 * @x: the value to be checked
 * @a: the alignment value
 */

#define aligned_at(x, a)        (((uintptr_t) (x) & ((uintptr_t) (a) - 1)) == 0)

/*
 * min - get the min value of two
 * @x: the 1st value
 * @y: the 2nd value
 */

#ifndef min
#define min(x, y)               ((x) < (y) ? (x) : (y))
#endif

/*
 * min - get the max value of two
 * @x: the 1st value
 * @y: the 2nd value
 */

#ifndef max
#define max(x, y)               ((x) > (y) ? (x) : (y))
#endif

/*
 * STATIC_ASSERT - assert at compile time
 * @e: the expression checked
 */

#define STATIC_ASSERT(e)        typedef int __static_assert [(e) ? 1 : -1]

/*
 * ARRAY_SIZE - get the number of elements for an array
 * @a: the array name
 */

#define ARRAY_SIZE(a)           (sizeof (a) / sizeof ((a) [0]))

/*
 * __CONCAT - make a new symbol by join two sub-strings
 * @s1: string 1
 * @s2: string 2
 */

#ifdef  __CONCAT
#undef  __CONCAT
#endif

#define __CONCAT_RAW(s1, s2)    s1 ## s2
#define __CONCAT(s1, s2)        __CONCAT_RAW (s1, s2)

/*
 * __CVTSTR - create a string as string -> "string"
 * @s: the input string
 */

#define __CVTSTR_RAW(s)         #s
#define __CVTSTR(s)             __CVTSTR_RAW (s)

/*
 * __fls_u32 - find the last set in a uint32_t
 * @x: the numeric value
 */

static __always_inline int __fls_u32 (uint32_t x)
    {
    return 31 - __clz_u32 (x);
    }

/*
 * __fls_u64 - find the last set in a uint64_t
 * @x: the numeric value
 */

static __always_inline int __fls_u64 (uint64_t x)
    {
    return 63 - __clz_u64 (x);
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __COMMON_H__ */

