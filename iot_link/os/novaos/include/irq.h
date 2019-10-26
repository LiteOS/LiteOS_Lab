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

#ifndef __IRQ_H__
#define __IRQ_H__

#include <stdbool.h>

#include <arch/irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern unsigned int int_cnt;

// TODO: int_cnt -> macro or func

static inline bool int_context (void)
    {
    return (int_cnt != 0) || int_context_arch ();
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IRQ_H__ */

