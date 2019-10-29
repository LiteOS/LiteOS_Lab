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

#ifndef __COMPILER_MSVC_H__
#define __COMPILER_MSVC_H__

/*
 * __unused - create a "unused" symbol
 */

#define __unused

/*
 * __noreturn - create a non-return routine
 */

#define __noreturn

/*
 * __weak - create a weak symbol
 */

#define __weak

/*
 * dummy - create a non-used variable
 * @type: the type of the new variable
 * @n: the id of this variable, used in the variable name
 */

#define dummy(type, n)                  type dummy##n

/*
 * likely - a condition is very likely to be true.
 * @exp: the bool exp
 */

#define likely(exp)                     (exp)

/*
 * unlikely - a condition is very unlikely to be true.
 * @exp: the bool exp.
 */

#define unlikely(exp)                   (exp)

/*
 * __section__ - place a symbol in a specific section
 * @name: the section name.
 */

#define __section__(name)

/*
 * __section_start__ - import to the start of a section
 * @name: the section name.
 */

#define __section_start__(name)         name

/*
 * __section_end__ - import to the end of a section
 * @name: the section name.
 */

#define __section_end__(name)           name

/*
 * __clz_u32 - counting the number of leading 0-bits in a uint32_t value
 * @x: the numeric value
 *
 * note: if x is 0, the result is undefined
 */

static inline int __clz_u32 (uint32_t x)
    {
    if (x == 0)
        {
        return 32;
        }

    return _lzcnt_u32  (x);
    }

/*
 * __ctz_u32 - counting the number of trailing 0-bits in a uint32_t value
 * @x: the numeric value
 *
 * note: if x is 0, the result is undefined
 */

static inline int __ctz_u32 (uint32_t x)
    {
    if (x == 0)
        {
        return 32;
        }

    return 32 - __clz_u32 (~(x) & ((x) - 1));
    }

/*
 * __clz_u64 - counting the number of leading 0-bits in a uint64_t value
 * @x: the numeric value
 *
 * note: if x is 0, the result is undefined
 */

static inline int __clz_u64 (uint64_t x)
    {
    if (x == 0)
        {
        return 64;
        }

    if ((x >> 32) == 0)
        {
        return 32 + __clz_u32 ((uint32_t) x);
        }
    else
        {
        return __clz_u32 ((uint32_t) (x >> 32));
        }
    }

/*
 * __ctz_u64 - counting the number of trailing 0-bits in a uint64_t value
 * @x: the numeric value
 *
 * note: if x is 0, the result is undefined
 */

static inline int __ctz_u64 (uint64_t x)
    {
    if (x == 0)
        {
        return 64;
        }

    return 64 - __clz_u64 (~(x) & ((x) - 1));
    }

#endif /* __COMPILER_MSVC_H__ */

