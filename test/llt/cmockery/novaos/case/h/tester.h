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

#ifndef __TESTER_H__
#define __TESTER_H__

#include <common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* typedefs */
typedef struct tester_statistics
    {
    uint32_t totals;
    uint32_t fails;
    } tester_statistics_t;

typedef struct tester_case
    {
    const char *name;
    uint32_t timeout;
    void (*entry) (struct tester_case *);
    void (*setup) (void);
    void (*teardown) (void);
    tester_statistics_t *stats;
    } tester_case_t;

/* macros */

#define TESTER_SECTION      tester

#define __TESTER_CASE_DEF(name, entry, setup, teardown, timeout, tag)           \
    static tester_statistics_t __CONCAT (stats, __CONCAT (entry, tag));         \
    const tester_case_t __CONCAT (entry, tag) __section__ (TESTER_SECTION) =    \
        {                                                                       \
        name,                                                                   \
        timeout,                                                                \
        entry,                                                                  \
        setup,                                                                  \
        teardown,                                                               \
        &__CONCAT (stats, __CONCAT (entry, tag))                                \
        }

#define TESTER_CASE_DEF(name, entry, setup, teardown, timeout) \
    __TESTER_CASE_DEF(name, entry, setup, teardown, timeout, __LINE__)

/* externs */

extern char __section_start__ (TESTER_SECTION) [];
extern char __section_end__   (TESTER_SECTION) [];


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CMDER_H__ */