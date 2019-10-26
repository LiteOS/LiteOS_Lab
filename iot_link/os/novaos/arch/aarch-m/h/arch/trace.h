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

#ifndef __AARCH_M_TRACE_H__
#define __AARCH_M_TRACE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */

/* typedefs */

/* externs */

#ifdef CONFIG_TRACE
extern void back_trace (uint32_t, uint32_t, uint32_t);
extern void call_trace (void);
#else
#define back_trace(...)
#define call_trace(...)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AARCH_M_TRACE_H__ */

