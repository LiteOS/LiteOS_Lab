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

#ifndef __MODULE_H__
#define __MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 0x00 ~ 0x1f kernel */

#define MID_CRITICAL        0x00
#define MID_TICK            0x01
#define MID_TASK            0x02
#define MID_MUTEX           0x03
#define MID_SEM             0x04
#define MID_EVENT           0x05
#define MID_TIMER           0x06
#define MID_OBJ             0x07

/* 0x06 ~ 0x0e reserved for now */

#define MID_MQ              0x0f

/* 0x20 ~ 0x3f other part of the core */

#define MID_HEAP            0x10
#define MID_MEM             0x11
#define MID_MEMPOOL         0x12
#define MID_MEMTRY          0x13
#define MID_DEFER           0x18
#define MID_ERRNO           0x19
#define MID_SYSCALL         0x1a
#define MID_KPRINTF         0x1b

#define MID_HAL             0x40

#define MID_MPU             0x41

/* used by customer in syscall */

#define MID_CUSTOMER        0xff

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MODULE_H__ */

