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

#ifndef __BUG_H__
#define __BUG_H__

#include <kconfig.h>

#ifdef CONFIG_DEBUG

#include <irq.h>
#include <kprintf.h>

#include <arch/trace.h>
#endif

/*
 * BUG - enter bug state
 *
 * usage: BUG (format, args), arguments is the same as kprintf
 */

#ifdef CONFIG_DEBUG
#define BUG(...)                                                                \
do                                                                              \
    {                                                                           \
                                                                                \
    /* disable interrupt to force kprintf use poll mode */                      \
                                                                                \
    (void) int_lock ();                                                         \
                                                                                \
    kprintf ("\nBUG, " __VA_ARGS__);                                            \
                                                                                \
    kprintf ("\n@ (file : %s, line : %d)", __FILE__, __LINE__);                 \
                                                                                \
    __bug ("");                                                                 \
    } while (0)
#else
#define BUG(...)
#endif

/*
 * BUG_ON - executive code and enter to bug state when the result is true
 *
 * code:   the code to executive, note, it will be executived always regardless
 *         to the macro of CONFIG_DEBUG, if the code is comperation only, compiler
 *         can do correct optimization
 *
 * examples:
 *
 *     BUG_ON (xxx != 0, "xxx is %d", xxx);
 *
 *     BUG_ON (xxx == 0, "xxx is not supported");
 */

#ifdef CONFIG_DEBUG
#define BUG_ON(code, ...)                                                       \
do                                                                              \
    {                                                                           \
    if (unlikely (code))                                                        \
        {                                                                       \
        BUG ("\"" __CVTSTR_RAW (code) "\" unexpected!\n" __VA_ARGS__);          \
        }                                                                       \
    } while (0)
#else
#define BUG_ON(cond, ...)                                                       \
do                                                                              \
    {                                                                           \
    (void) (cond);                                                              \
    } while (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void __bug (const char *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BUG_H__ */

