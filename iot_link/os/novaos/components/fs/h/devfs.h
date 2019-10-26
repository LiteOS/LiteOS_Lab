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

#ifndef __DEVFS_H__
#define __DEVFS_H__

#include <vfs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* typedefs */

struct devfs_ops
    {
    int    (* open)  (uintptr_t);
    int    (* close) (uintptr_t);
    int    (* read)  (uintptr_t, char *, size_t);
    int    (* write) (uintptr_t, const char *, size_t);
    int    (* lseek) (uintptr_t, int, int);
    int    (* ioctl) (uintptr_t, int, va_list);
    int    (* sync)  (uintptr_t);
    int    (* stat)  (uintptr_t, struct stat *);
    size_t (* size)  (uintptr_t);
    };

/* externs */

extern int devfs_add_file (const char *, const struct devfs_ops *, uintptr_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DEVFS_H__ */
