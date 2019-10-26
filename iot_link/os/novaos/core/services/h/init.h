/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 *      http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#ifndef __INIT_H__
#define __INIT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <compiler.h>

/* typedefs */

typedef int (* init_pfn) (void);

/* macros */

/*
 * MODULE_INIT (group, init) - add a module init routine
 * @group: the init group name
 * @init:  the init routine
 *
 * supported groups for now:
 *
 *     *) cpu
 *     *) board
 *     *) prekernel
 *     *) kernel
 *     *) postkernel
 *     *) hal
 *     *) bus
 *     *) driver
 *     *) postdriver
 *     *) user
 *
 * the init senquence is the same as listed, and user is inited in task context
 * by deferred job task
 */

#define MODULE_INIT(group, init)    \
    const init_pfn __init_##group##_##init __section__ (init_##group) = init

extern  __noreturn void             kernel_init (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INIT_H__ */

