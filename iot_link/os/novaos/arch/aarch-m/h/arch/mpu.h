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

#ifndef __AARCH_M_MPU_H__
#define __AARCH_M_MPU_H__

#include <stdint.h>

#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

/*      attr macro defination                                  privileged unprivileged */

#define MPU_ATTR_______             (0u << 24 | 1u << 28)   /* none       none         */
#define MPU_ATTR_RW____             (1u << 24 | 1u << 28)   /* RW         none         */
#define MPU_ATTR_RW_R__             (2u << 24 | 1u << 28)   /* RW         R            */
#define MPU_ATTR_RW_RW_             (3u << 24 | 1u << 28)   /* RW         RW           */
#define MPU_ATTR_R_____             (5u << 24 | 1u << 28)   /* R          none         */
#define MPU_ATTR_R__R__             (6u << 24 | 1u << 28)   /* R          R            */

#define MPU_ATTR_RWX___             (1u << 24)              /* RWX        none         */
#define MPU_ATTR_RWXR_X             (2u << 24)              /* RWX        RX           */
#define MPU_ATTR_RWXRWX             (3u << 24)              /* RWX        RWX          */
#define MPU_ATTR_R_X___             (5u << 24)              /* RX         none         */
#define MPU_ATTR_R_XR_X             (6u << 24)              /* RX         RX           */


#define MPU_ATTR_STRONGLY           (4u << 16) /* strongly always s, so set S */
#define MPU_ATTR_DEVICE             (5u << 16) /* device always s, so set S */
#define MPU_ATTR_WT                 (2u << 16)
#define MPU_ATTR_WB                 (3u << 16)

#define ERRNO_MPU_ILLEGAL_REGIONS   ERRNO_JOIN (MID_MPU, 1)
#define ERRNO_MPU_ILLEGAL_OPERATION ERRNO_JOIN (MID_MPU, 2)
#define ERRNO_MPU_UNAVAILABLE       ERRNO_JOIN (MID_MPU, 3)

/* typedefs */

struct mpu_region
    {
    uint32_t addr;
    uint32_t size;
    uint32_t attr;
    };

struct mpu_entry
    {
    uint32_t rbar;
    uint32_t rasr;
    };

/* externs */

extern task_id mpu_task_spawn (const char *, uint8_t, uint32_t, size_t,
                               int (*) (uintptr_t), uintptr_t, struct mpu_region *);
extern int     mpu_region_add (task_id, uint32_t, uint32_t, uint32_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AARCH_M_MPU_H__ */
