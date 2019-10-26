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

#ifndef __WARN_H__
#define __WARN_H__

#include <kconfig.h>

#include <critical.h>
#include <errno.h>

#ifdef CONFIG_DEBUG     // DEBUG select KPRINTF
#include <kprintf.h>
#endif

#include <arch/trace.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * WARN - print a warning message
 *
 * usage: WARN (format, args), arguments is the same as kprintf
 */

#ifdef CONFIG_DEBUG
#define WARN(...)                                                               \
do                                                                              \
    {                                                                           \
    if (!in_critical ())                                                        \
        {                                                                       \
        uint32_t err = errno;                                                   \
                                                                                \
        kprintf ("\nWARNING, " __VA_ARGS__);                                    \
        kprintf ("\n@ (file : %s, line : %d)", __FILE__, __LINE__);             \
        call_trace ();                                                          \
                                                                                \
        errno = err;                                                            \
        }                                                                       \
    } while (0)
#else
#define WARN(...)
#endif

/*
 * WARN_ON - executive code and print a warning message when the result is true
 *
 * code:   the code to executive, note, it will be executived always regardless
 *         to the macro of CONFIG_DEBUG, if the code is comperation only, compiler
 *         can do correct optimization
 * action: action when the result is true, can be a code block or more than one
 *         sentence without ',' and can be empty (do nothing)
 *
 * examples:
 *
 *     WARN_ON (xxx != 0,
 *              return,                         // action
 *              "xxx is %d", xxx);
 *
 *     WARN_ON (xxx != 0, , "xxx is %d", xxx);  // action is empty
 *
 *     WARN_ON (xxx == 0,
 *              errno = ERR_XXX; return -1,     // action is more than one
 *              "xxx is not supported");
 *
 *     WARN_ON (xxx == 0,
 *              {
 *              errno = ERR_XXX;
 *              return -1;
 *              },
 *              "xxx is not supported");
 */

#define WARN_ON(code, action, ...)                                              \
do                                                                              \
    {                                                                           \
    if (unlikely (code))                                                        \
        {                                                                       \
        WARN ("\"" __CVTSTR_RAW (code) "\" unexpected!\n" __VA_ARGS__);         \
        action;                                                                 \
        }                                                                       \
    } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WARN_H__ */

