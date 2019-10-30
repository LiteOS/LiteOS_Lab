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

#ifndef __AARCH_M_EXC_H__
#define __AARCH_M_EXC_H__

#include "regset.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */

#define EXC_VEC_RESET               1
#define EXC_VEC_NMI                 2
#define EXC_VEC_HARDFAULT           3
#define EXC_VEC_MEMMANAGE           4
#define EXC_VEC_BUSFAULT            5
#define EXC_VEC_USABEFAULT          6
#define EXC_VEC_SVC                 11
#define EXC_VEC_DEBUGMONITOR        12
#define EXC_VEC_PENDSV              14
#define EXC_VEC_SYSTICK             15

/* typedefs */

typedef struct exc_info
    {
    struct esp * esp;
    uint32_t     r4;
    uint32_t     r5;
    uint32_t     r6;
    uint32_t     r7;
    uint32_t     r8;
    uint32_t     r9;
    uint32_t     r10;
    uint32_t     r11;
    uint32_t     exclr;
#ifdef CONFIG_AARCH_M_MAINLINE
    uint32_t     shcsr;
    uint32_t     cfsr;
    uint32_t     hfsr;
    uint32_t     dfsr;
    uint32_t     mmfar;
    uint32_t     bfar;
#endif
    } exc_info_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AARCH_M_EXC_H__ */

