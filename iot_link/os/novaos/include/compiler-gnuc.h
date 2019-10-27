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

#ifndef __COMPILER_GNUC_H__
#define __COMPILER_GNUC_H__

#include <limits.h>

#include <kconfig.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef  __KEIL__
#define SECTION_NAME_PREFIX
#define SECTION_SYM_PREFIX
#define SECTION_SYM_START               $$Base
#define SECTION_SYM_END                 $$Limit
#else
#define SECTION_NAME_PREFIX             "."
#define SECTION_SYM_PREFIX              __
#define SECTION_SYM_START               _start__
#define SECTION_SYM_END                 _end__
#endif

/*
 * __unused - create a "unused" symbol
 */

#ifndef __unused
#define __unused                        __attribute__ ((unused))
#endif

/*
 * __noreturn - create a non-return routine
 */

#ifndef __noreturn
#define __noreturn                      __attribute__ ((noreturn))
#endif

/*
 * __weak - create a weak symbol
 */

#ifndef __weak
#define __weak                          __attribute__ ((weak))
#endif

/*
 * __naked - create a naked asm function
 */

#ifndef __naked
#define __naked                         __attribute__ ((naked))
#endif

/*
 * __always_inline - create always inlined function
 */

#ifndef __always_inline
#define __always_inline                 __attribute__ ((always_inline, unused)) inline
#endif

/*
 * __noinline - create no inlined function
 */

#ifndef __noinline
#define __noinline                      __attribute__ ((noinline))
#endif

/*
 * dummy - create a non-used variable
 * @type: the type of the new variable
 * @n: the id of this variable, used in the variable name
 */

#define dummy(type, n)                  type dummy##n __unused

/*
 * likely - a condition is very likely to be true.
 * @exp: the bool exp
 */

#define likely(exp)                     __builtin_expect(!!(exp), 1)

/*
 * unlikely - a condition is very unlikely to be true.
 * @exp: the bool exp.
 */

#define unlikely(exp)                   __builtin_expect(!!(exp), 0)

/*
 * __section__ - place a symbol in a specific section
 * @name: the section name.
 */

#define __section__(name)               \
    __attribute__ ((section (SECTION_NAME_PREFIX __CVTSTR (name))))

/*
 * __section_start__ - import to the start of a section
 * @name: the section name.
 */

#define __section_start__(name)         \
    __CONCAT (__CONCAT (SECTION_SYM_PREFIX, name), SECTION_SYM_START)

/*
 * __section_end__ - import to the end of a section
 * @name: the section name.
 */

#define __section_end__(name)           \
    __CONCAT (__CONCAT (SECTION_SYM_PREFIX, name), SECTION_SYM_END)

/*
 * select the correct GNUC builtin counting zero routines according to the
 * platform
 */

#if   UINT_MAX  == 0xffffffffu

/* unsigned int is just uint32_t, use the int routines */

#define __builtin_clz_u32 __builtin_clz
#define __builtin_ctz_u32 __builtin_ctz
#define __builtin_int32_t int

#elif ULONG_MAX == 0xfffffffful

/* unsigned long is just uint32_t, use the int routines */

#define __builtin_clz_u32 __builtin_clzl
#define __builtin_ctz_u32 __builtin_ctzl
#define __builtin_int32_t long

#else
#error "platform not supported!"
#endif

/*
 * __clz_u32 - counting the number of leading 0-bits in a uint32_t value
 * @x: the numeric value
 *
 * note: if x is 0, the result is undefined
 */

static __always_inline int __clz_u32 (uint32_t x)
    {
    return __builtin_clz_u32 ((unsigned __builtin_int32_t) x);
    }

/*
 * __ctz_u32 - counting the number of trailing 0-bits in a uint32_t value
 * @x: the numeric value
 *
 * note: if x is 0, the result is undefined
 */

static __always_inline int __ctz_u32 (uint32_t x)
    {
    return __builtin_ctz_u32 ((unsigned __builtin_int32_t) x);
    }

#if ULLONG_MAX != 0xffffffffffffffffull
#error "platform not supported!"
#endif

/*
 * __clz_u64 - counting the number of leading 0-bits in a uint64_t value
 * @x: the numeric value
 *
 * note: if x is 0, the result is undefined
 */

static __always_inline int __clz_u64 (uint64_t x)
    {
    return __builtin_clzll ((unsigned long long) x);
    }

/*
 * __ctz_u64 - counting the number of trailing 0-bits in a uint32_t value
 * @x: the numeric value
 *
 * note: if x is 0, the result is undefined
 */

static __always_inline int __ctz_u64 (uint64_t x)
    {
    return __builtin_ctzll ((unsigned long long) x);
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __COMPILER_GNUC_H__ */

